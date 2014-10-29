
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include <check.h>
#include <ptab.h>

/* Common data */

static struct ptab p;


/* Common fixtures */

static void fixture_init_default(void)
{
	ptab_init(&p, NULL);
}

static void fixture_init_begin_columns(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
}

static void fixture_init_define_columns(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Column A", NULL, PTAB_STRING);
	ptab_define_column(&p, "Column B", "%d", PTAB_INTEGER);
}

static void fixture_init_end_columns(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Column A", NULL, PTAB_STRING);
	ptab_define_column(&p, "Column B", "%d", PTAB_INTEGER);
	ptab_end_columns(&p);
}

static void fixture_init_begin_row_s(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Column A", NULL, PTAB_STRING);
	ptab_define_column(&p, "Column B", NULL, PTAB_STRING);
	ptab_end_columns(&p);
	ptab_begin_row(&p);
}

static void fixture_init_begin_row_i(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Column A", "%d", PTAB_INTEGER);
	ptab_define_column(&p, "Column B", "%d", PTAB_INTEGER);
	ptab_end_columns(&p);
	ptab_begin_row(&p);
}

static void fixture_init_begin_row_f(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Column A", "%f", PTAB_FLOAT);
	ptab_define_column(&p, "Column B", "%f", PTAB_FLOAT);
	ptab_end_columns(&p);
	ptab_begin_row(&p);
}

static void fixture_init_add_data(void)
{
	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Column A", NULL, PTAB_STRING);
	ptab_define_column(&p, "Column B", "%d", PTAB_INTEGER);
	ptab_define_column(&p, "Column C", "%f", PTAB_FLOAT);
	ptab_end_columns(&p);
	ptab_begin_row(&p);
	ptab_add_row_data_s(&p, "String");
	ptab_add_row_data_i(&p, 5);
	ptab_add_row_data_f(&p, 7.5);
}

static void fixture_init_rows(void)
{
	ptab_init(&p, NULL);

	ptab_begin_columns(&p);
	ptab_define_column(&p, "String", NULL, PTAB_STRING);
	ptab_define_column(&p, "Integer", "%d", PTAB_INTEGER);
	ptab_define_column(&p, "Float", "%f", PTAB_FLOAT);
	ptab_end_columns(&p);

	ptab_begin_row(&p);
	ptab_add_row_data_s(&p, "Val1");
	ptab_add_row_data_i(&p, 5);
	ptab_add_row_data_f(&p, 7.5);
	ptab_end_row(&p);

	ptab_begin_row(&p);
	ptab_add_row_data_s(&p, "LongerString");
	ptab_add_row_data_i(&p, 150);
	ptab_add_row_data_f(&p, 20.137);
	ptab_end_row(&p);
}

static void fixture_free_default(void)
{
	ptab_free(&p);
}


/* Some helper functions */

static void *helper_alloc(size_t size, void *opaque)
{
	(void)opaque;

	return malloc(size);
}

static void *helper_null_alloc(size_t size, void *opaque)
{
	(void)size;
	(void)opaque;

	return NULL;
}

static void helper_free(void *ptr, void *opaque)
{
	(void)opaque;

	free(ptr);
}


/* Version test case */

START_TEST (test_version_string)
{
	const char *verstr;

	verstr = ptab_version_string();
	ck_assert_str_eq(verstr, PTAB_VERSION_STRING);
}
END_TEST

START_TEST (test_version)
{
	int major, minor, patch;

	ptab_version(&major, &minor, &patch);
	ck_assert_int_eq(major, PTAB_VERSION_MAJOR);
	ck_assert_int_eq(minor, PTAB_VERSION_MINOR);
	ck_assert_int_eq(patch, PTAB_VERSION_PATCH);
}
END_TEST


/* Init test case */

START_TEST (test_init)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = helper_alloc;
	pa.free_func = helper_free;
	pa.opaque = NULL;

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);

	/* check allocator */
	ck_assert(p.allocator.alloc_func == pa.alloc_func);
	ck_assert(p.allocator.free_func == pa.free_func);
	ck_assert(p.allocator.opaque == pa.opaque);

	ptab_free(&p);
}
END_TEST

START_TEST (test_init_no_allocator)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = helper_alloc;
	pa.free_func = helper_free;

	/* 
	 * assign the allocators in the ptable so we can
	 * see if they change during the init() call
	 */
	p.allocator.alloc_func = pa.alloc_func;
	p.allocator.free_func = pa.free_func;

	err = ptab_init(&p, NULL);
	ck_assert_int_eq(err, PTAB_OK);

	/* make sure the allocators changed */
	ck_assert(p.allocator.alloc_func != pa.alloc_func);
	ck_assert(p.allocator.free_func != pa.free_func);

	ptab_free(&p);
}
END_TEST

START_TEST (test_init_null)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	/* set some non-NULL pointers for the allocators */
	pa.alloc_func = helper_alloc;
	pa.free_func = helper_free;
	pa.opaque = NULL;

	/* non-NULL params and non-NULL allocators are good */
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_OK);
	ptab_free(&p);

	/* ensure NULL parameter causes error */
	err = ptab_init(NULL, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* set allocator to NULL and check for error */
	pa.alloc_func = NULL;
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);

	/* set free to NULL and check for error */
	pa.alloc_func = helper_alloc;
	pa.free_func = NULL;
	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_init_nomem)
{
	struct ptab p;
	struct ptab_allocator pa;
	int err;

	pa.alloc_func = helper_null_alloc;
	pa.free_func = helper_free;
	pa.opaque = NULL;

	err = ptab_init(&p, &pa);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST


/* Free test case */

START_TEST (test_free)
{
	int err;

	err = ptab_free(&p);
	ck_assert_int_eq(err, PTAB_OK);

	ck_assert(p.internal == NULL);
}
END_TEST

START_TEST (test_free_null)
{
	int err;

	err = ptab_free(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_free_order)
{
	struct ptab p;
	int err;

	p.internal = NULL;

	err = ptab_free(&p);
	ck_assert_int_eq(err, PTAB_EORDER);
}
END_TEST


/* Column test cases */

START_TEST (test_begin_columns)
{
	int err;

	err = ptab_begin_columns(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_begin_columns_null)
{
	int err;

	err = ptab_begin_columns(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_begin_columns_order)
{
	int err;

	ptab_begin_columns(&p);

	/*
	 * this should fail because begin_columns should
	 * only be called immediately after init, it
	 * should fail with EORDER for any other case
	 */
	err = ptab_begin_columns(&p);
	ck_assert_int_eq(err, PTAB_EORDER);
}
END_TEST

START_TEST (test_define_column)
{
	int err;

	err = ptab_define_column(&p, "Column", "%d units", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_define_column_null)
{
	int err;

	err = ptab_define_column(NULL, "Column", "%d units", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_define_column(&p, NULL, "%d units", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_define_column_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);

	err = ptab_define_column(&p, "Column", "%d", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_define_column_type)
{
	int err;

	err = ptab_define_column(&p, "Column", "hi", PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_STRING | PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ETYPE);

	err = ptab_define_column(&p, "Column", "hi", PTAB_STRING | PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_ETYPE);

	err = ptab_define_column(&p, "Column", "hi", PTAB_INTEGER | PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_ETYPE);

	err = ptab_define_column(&p, "Column", "hi",
		PTAB_INTEGER | PTAB_FLOAT | PTAB_STRING);
	ck_assert_int_eq(err, PTAB_ETYPE);
}
END_TEST

START_TEST (test_define_column_format_null)
{
	int err;

	err = ptab_define_column(&p, "Column", NULL, PTAB_INTEGER);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_define_column(&p, "Column", NULL, PTAB_FLOAT);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_define_column(&p, "Column", NULL, PTAB_STRING);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_define_column_align)
{
	int err;

	err = ptab_define_column(&p, "Column", "hi", PTAB_INTEGER | PTAB_ALIGN_LEFT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_INTEGER | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_STRING | PTAB_ALIGN_LEFT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_STRING | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_FLOAT | PTAB_ALIGN_LEFT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi", PTAB_FLOAT | PTAB_ALIGN_RIGHT);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_define_column(&p, "Column", "hi",
		PTAB_FLOAT | PTAB_ALIGN_RIGHT | PTAB_ALIGN_LEFT);
	ck_assert_int_eq(err, PTAB_EALIGN);
}
END_TEST

START_TEST (test_define_column_nomem)
{
	int err;

	p.allocator.alloc_func = helper_null_alloc;

	err = ptab_define_column(&p, "Column", NULL, PTAB_STRING);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (test_end_columns)
{
	int err;

	err = ptab_end_columns(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_end_columns_null)
{
	int err;

	err = ptab_end_columns(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_end_columns_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);

	err = ptab_end_columns(&p);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_end_columns_nocolumns)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);

	err = ptab_end_columns(&p);
	ck_assert_int_eq(err, PTAB_ENOCOLUMNS);

	ptab_free(&p);
}
END_TEST


/* Row test cases */

START_TEST (test_begin_row)
{
	int err;

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_begin_row_null)
{
	int err;

	err = ptab_begin_row(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_begin_row_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_begin_row_nomem)
{
	int err;

	p.allocator.alloc_func = helper_null_alloc;

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST


/* Add Row Data Tests */

START_TEST (test_add_row_data_s)
{
	int err;

	err = ptab_add_row_data_s(&p, "String");
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_add_row_data_s_multiple)
{
	int err;

	err = ptab_add_row_data_s(&p, "String");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_s(&p, "String Too");
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_add_row_data_s_toomany)
{
	int err;

	err = ptab_add_row_data_s(&p, "String");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_s(&p, "String Too");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_s(&p, "String Again");
	ck_assert_int_eq(err, PTAB_ENUMCOLUMNS);
}
END_TEST

START_TEST (test_add_row_data_s_nomem)
{
	int err;

	p.allocator.alloc_func = helper_null_alloc;

	err = ptab_add_row_data_s(&p, "String");
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (test_add_row_data_s_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);

	err = ptab_add_row_data_s(&p, "Fail");
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_add_row_data_s_type)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Float", "%f", PTAB_FLOAT);
	ptab_end_columns(&p);
	ptab_begin_row(&p);

	err = ptab_add_row_data_s(&p, "Fail");
	ck_assert_int_eq(err, PTAB_ETYPE);

	ptab_free(&p);
}
END_TEST

START_TEST (test_add_row_data_s_null)
{
	int err;

	err = ptab_add_row_data_s(NULL, "Fail");
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_add_row_data_s(&p, NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_add_row_data_i)
{
	int err;

	err = ptab_add_row_data_i(&p, 5);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_add_row_data_i_multiple)
{
	int err;

	err = ptab_add_row_data_i(&p, 5);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_i(&p, 6);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_add_row_data_i_toomany)
{
	int err;

	err = ptab_add_row_data_i(&p, 1);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_i(&p, 2);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_i(&p, 3);
	ck_assert_int_eq(err, PTAB_ENUMCOLUMNS);
}
END_TEST

START_TEST (test_add_row_data_i_nomem)
{
	int err;

	p.allocator.alloc_func = helper_null_alloc;

	err = ptab_add_row_data_i(&p, 3);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (test_add_row_data_i_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);

	err = ptab_add_row_data_i(&p, 1);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_add_row_data_i_type)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Float", "%f", PTAB_FLOAT);
	ptab_end_columns(&p);
	ptab_begin_row(&p);

	err = ptab_add_row_data_i(&p, 5);
	ck_assert_int_eq(err, PTAB_ETYPE);

	ptab_free(&p);
}
END_TEST

START_TEST (test_add_row_data_i_null)
{
	int err;

	err = ptab_add_row_data_i(NULL, 1);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_add_row_data_f)
{
	int err;

	err = ptab_add_row_data_f(&p, 5.0);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_add_row_data_f_multiple)
{
	int err;

	err = ptab_add_row_data_f(&p, 5.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_f(&p, 6.0);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_add_row_data_f_toomany)
{
	int err;

	err = ptab_add_row_data_f(&p, 1.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_f(&p, 2.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_add_row_data_f(&p, 3.0);
	ck_assert_int_eq(err, PTAB_ENUMCOLUMNS);
}
END_TEST

START_TEST (test_add_row_data_f_nomem)
{
	int err;

	p.allocator.alloc_func = helper_null_alloc;

	err = ptab_add_row_data_f(&p, 3.0);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (test_add_row_data_f_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);

	err = ptab_add_row_data_f(&p, 1.0);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_add_row_data_f_type)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Integer", "%d", PTAB_INTEGER);
	ptab_end_columns(&p);
	ptab_begin_row(&p);

	err = ptab_add_row_data_f(&p, 5.0);
	ck_assert_int_eq(err, PTAB_ETYPE);

	ptab_free(&p);
}
END_TEST

START_TEST (test_add_row_data_f_null)
{
	int err;

	err = ptab_add_row_data_f(NULL, 1.0);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_end_row)
{
	int err;

	err = ptab_end_row(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (test_end_row_null)
{
	int err;

	err = ptab_end_row(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (test_end_row_order)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Integer", "%d", PTAB_INTEGER);
	ptab_end_columns(&p);
	err = ptab_end_row(&p);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(&p);
}
END_TEST

START_TEST (test_end_row_multiple)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Integer", "%d", PTAB_INTEGER);
	ptab_end_columns(&p);

	ptab_begin_row(&p);
	ptab_add_row_data_i(&p, 5);
	ptab_end_row(&p);

	ptab_begin_row(&p);
	ptab_add_row_data_i(&p, 23);
	err = ptab_end_row(&p);
	ck_assert_int_eq(err, PTAB_OK);

	ptab_free(&p);
}
END_TEST

START_TEST (test_end_row_complete)
{
	struct ptab p;
	int err;

	ptab_init(&p, NULL);
	ptab_begin_columns(&p);
	ptab_define_column(&p, "Integer", "%d", PTAB_INTEGER);
	ptab_define_column(&p, "Integer", "%d", PTAB_INTEGER);
	ptab_end_columns(&p);

	ptab_begin_row(&p);
	ptab_add_row_data_i(&p, 5);
	err = ptab_end_row(&p);
	ck_assert_int_eq(err, PTAB_ECOMPLETE);

	ptab_free(&p);
}
END_TEST


/* to_string Test Cases */

START_TEST (test_to_string)
{
}
END_TEST

START_TEST (test_to_string_order)
{
}
END_TEST

START_TEST (test_to_string_nomem)
{
}
END_TEST

START_TEST (test_to_string_flags)
{
}
END_TEST


/* Suite definition */

Suite *get_libptab_suite(void)
{
	Suite *s;
	TCase *tc_version;
	TCase *tc_init;
	TCase *tc_free;
	TCase *tc_begin_columns;
	TCase *tc_define_column;
	TCase *tc_end_columns;
	TCase *tc_begin_row;
	TCase *tc_add_row_data_s;
	TCase *tc_add_row_data_i;
	TCase *tc_add_row_data_f;
	TCase *tc_end_row;
	TCase *tc_to_string;

	s = suite_create("libptab Test Suite");

	/* create test cases */
	tc_version = tcase_create("Version");
	tcase_add_test(tc_version, test_version_string);
	tcase_add_test(tc_version, test_version);
	suite_add_tcase(s, tc_version);

	tc_init = tcase_create("Init");
	tcase_add_test(tc_init, test_init);
	tcase_add_test(tc_init, test_init_no_allocator);
	tcase_add_test(tc_init, test_init_null);
	tcase_add_test(tc_init, test_init_nomem);
	suite_add_tcase(s, tc_init);

	tc_free = tcase_create("Free");
	tcase_add_checked_fixture(tc_free, fixture_init_default, NULL);
	tcase_add_test(tc_free, test_free);
	tcase_add_test(tc_free, test_free_null);
	tcase_add_test(tc_free, test_free_order);
	suite_add_tcase(s, tc_free);

	tc_begin_columns = tcase_create("Begin Columns");
	tcase_add_checked_fixture(tc_begin_columns,
		fixture_init_default, fixture_free_default);
	tcase_add_test(tc_begin_columns, test_begin_columns);
	tcase_add_test(tc_begin_columns, test_begin_columns_null);
	tcase_add_test(tc_begin_columns, test_begin_columns_order);
	suite_add_tcase(s, tc_begin_columns);

	tc_define_column = tcase_create("Define Column");
	tcase_add_checked_fixture(tc_define_column,
		fixture_init_begin_columns, fixture_free_default);
	tcase_add_test(tc_define_column, test_define_column);
	tcase_add_test(tc_define_column, test_define_column_null);
	tcase_add_test(tc_define_column, test_define_column_order);
	tcase_add_test(tc_define_column, test_define_column_type);
	tcase_add_test(tc_define_column, test_define_column_format_null);
	tcase_add_test(tc_define_column, test_define_column_align);
	tcase_add_test(tc_define_column, test_define_column_nomem);
	suite_add_tcase(s, tc_define_column);

	tc_end_columns = tcase_create("End Columns");
	tcase_add_checked_fixture(tc_end_columns,
		fixture_init_define_columns, fixture_free_default);
	tcase_add_test(tc_end_columns, test_end_columns);
	tcase_add_test(tc_end_columns, test_end_columns_null);
	tcase_add_test(tc_end_columns, test_end_columns_order);
	tcase_add_test(tc_end_columns, test_end_columns_nocolumns);
	suite_add_tcase(s, tc_end_columns);

	tc_begin_row = tcase_create("Begin Row");
	tcase_add_checked_fixture(tc_begin_row,
		fixture_init_end_columns, fixture_free_default);
	tcase_add_test(tc_begin_row, test_begin_row);
	tcase_add_test(tc_begin_row, test_begin_row_null);
	tcase_add_test(tc_begin_row, test_begin_row_order);
	tcase_add_test(tc_begin_row, test_begin_row_nomem);
	suite_add_tcase(s, tc_begin_row);

	tc_add_row_data_s = tcase_create("Add Row Data (Strings)");
	tcase_add_checked_fixture(tc_add_row_data_s,
		fixture_init_begin_row_s, fixture_free_default);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s_multiple);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s_toomany);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s_nomem);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s_order);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s_type);
	tcase_add_test(tc_add_row_data_s, test_add_row_data_s_null);
	suite_add_tcase(s, tc_add_row_data_s);

	tc_add_row_data_i = tcase_create("Add Row Data (Integer)");
	tcase_add_checked_fixture(tc_add_row_data_i,
		fixture_init_begin_row_i, fixture_free_default);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i_multiple);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i_toomany);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i_nomem);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i_order);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i_type);
	tcase_add_test(tc_add_row_data_i, test_add_row_data_i_null);
	suite_add_tcase(s, tc_add_row_data_i);

	tc_add_row_data_f = tcase_create("Add Row Data (Float)");
	tcase_add_checked_fixture(tc_add_row_data_f,
		fixture_init_begin_row_f, fixture_free_default);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f_multiple);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f_toomany);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f_nomem);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f_order);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f_type);
	tcase_add_test(tc_add_row_data_f, test_add_row_data_f_null);
	suite_add_tcase(s, tc_add_row_data_f);

	tc_end_row = tcase_create("End Row");
	tcase_add_checked_fixture(tc_end_row,
		fixture_init_add_data, fixture_free_default);
	tcase_add_test(tc_end_row, test_end_row);
	tcase_add_test(tc_end_row, test_end_row_null);
	tcase_add_test(tc_end_row, test_end_row_order);
	tcase_add_test(tc_end_row, test_end_row_multiple);
	tcase_add_test(tc_end_row, test_end_row_complete);
	suite_add_tcase(s, tc_end_row);

	tc_to_string = tcase_create("To String");
	tcase_add_checked_fixture(tc_to_string,
		fixture_init_rows, fixture_free_default);
	tcase_add_test(tc_to_string, test_to_string);
	tcase_add_test(tc_to_string, test_to_string_order);
	tcase_add_test(tc_to_string, test_to_string_nomem);
	tcase_add_test(tc_to_string, test_to_string_flags);
	suite_add_tcase(s, tc_to_string);

	return s;
}


