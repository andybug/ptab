
#include <check.h>
#include <ptab.h>
#include "../src/internal.h"

static ptab p;
static int err;

static void fixture_init_columns(void)
{
	memset(&p, 0, sizeof(ptab));
	ptab_init(&p, NULL);

	ptab_column(&p, "StringColumn", PTAB_STRING);
	ptab_column(&p, "IntegerColumn", PTAB_INTEGER);
	ptab_column(&p, "FloatColumn", PTAB_FLOAT);
}

static void fixture_init_begin_row(void)
{
	fixture_init_columns();
	ptab_begin_row(&p);
}

static void fixture_free(void)
{
	ptab_free(&p);
}

static void *alloc_null(size_t size, void *opaque)
{
	(void)size;
	(void)opaque;

	return NULL;
}

START_TEST (begin_row_default)
{
	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (begin_row_nocolumns)
{
	ptab p;

	ptab_init(&p, NULL);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_ENOCOLUMNS);

	ptab_free(&p);
}
END_TEST

START_TEST (begin_row_nomem)
{
	size_t alloc_size;

	p.allocator.alloc_func = alloc_null;

	/* ugly hack */
	alloc_size = p.internal->alloc_tree->avail;
	ptab_alloc(&p, alloc_size);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_ENOMEM);
}
END_TEST

START_TEST (begin_row_notfinished)
{
	/* TODO: probably need to implement this after end_row */
}
END_TEST

START_TEST (begin_row_null)
{
	err = ptab_begin_row(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (begin_row_init)
{
	ptab p;

	p.internal = NULL;

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_EINIT);
}
END_TEST

START_TEST (begin_row_alreadybegan)
{
	ptab_begin_row(&p);

	err = ptab_begin_row(&p);
	ck_assert_int_eq(err, PTAB_EROWBEGAN);
}
END_TEST

START_TEST (row_data_s_default)
{
	err = ptab_row_data_s(&p, "Row data");
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

TCase *begin_row_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Begin Row");
	tcase_add_checked_fixture(tc, fixture_init_columns, fixture_free);
	tcase_add_test(tc, begin_row_default);
	tcase_add_test(tc, begin_row_nocolumns);
	tcase_add_test(tc, begin_row_nomem);
	tcase_add_test(tc, begin_row_notfinished);
	tcase_add_test(tc, begin_row_null);
	tcase_add_test(tc, begin_row_init);
	tcase_add_test(tc, begin_row_alreadybegan);

	return tc;
}

TCase *row_data_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Row Data");
	tcase_add_checked_fixture(tc, fixture_init_begin_row, fixture_free);
	tcase_add_test(tc, row_data_s_default);

	return tc;
}
