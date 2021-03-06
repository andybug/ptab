
#include <check.h>
#include <ptab.h>

#include "../src/internal.h"

static ptab_t *p;
static int err;

static void fixture_init_columns(void)
{
	p = ptab_init(NULL);

	ptab_column(p, "StringColumn", PTAB_STRING);
	ptab_column(p, "IntegerColumn", PTAB_INTEGER);
	ptab_column(p, "FloatColumn", PTAB_FLOAT);
}

static void fixture_begin_row_s(void)
{
	p = ptab_init(NULL);

	ptab_column(p, "S", PTAB_STRING);
	ptab_column(p, "I", PTAB_INTEGER);
	ptab_column(p, "F", PTAB_FLOAT);

	ptab_begin_row(p);
}

static void fixture_begin_row_i(void)
{
	p = ptab_init(NULL);

	ptab_column(p, "I", PTAB_INTEGER);
	ptab_column(p, "S", PTAB_STRING);
	ptab_column(p, "F", PTAB_FLOAT);

	ptab_begin_row(p);
}

static void fixture_begin_row_f(void)
{
	p = ptab_init(NULL);

	ptab_column(p, "F", PTAB_FLOAT);
	ptab_column(p, "I", PTAB_INTEGER);
	ptab_column(p, "S", PTAB_STRING);

	ptab_begin_row(p);
}

static void fixture_free(void)
{
	ptab_free(p);
}

START_TEST (begin_row_default)
{
	err = ptab_begin_row(p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (begin_row_nocolumns)
{
	ptab_t *p;

	p = ptab_init(NULL);

	err = ptab_begin_row(p);
	ck_assert_int_eq(err, PTAB_EORDER);

	ptab_free(p);
}
END_TEST

START_TEST (begin_row_nomem)
{
	ptab__mem_disable(p);

	err = ptab_begin_row(p);
	ck_assert_int_eq(err, PTAB_EMEM);

	ptab__mem_enable(p);
}
END_TEST

START_TEST (begin_row_notfinished)
{
	ptab_begin_row(p);

	err = ptab_begin_row(p);
	ck_assert_int_eq(err, PTAB_EORDER);
}
END_TEST

START_TEST (begin_row_null)
{
	err = ptab_begin_row(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (begin_row_alreadybegan)
{
	ptab_begin_row(p);

	err = ptab_begin_row(p);
	ck_assert_int_eq(err, PTAB_EORDER);
}
END_TEST

START_TEST (row_data_s_default)
{
	err = ptab_row_data_s(p, "Row data");
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (row_data_s_null)
{
	err = ptab_row_data_s(NULL, "Row data");
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_row_data_s(p, NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (row_data_s_nomem)
{
	ptab__mem_disable(p);

	err = ptab_row_data_s(p, "Row data");
	ck_assert_int_eq(err, PTAB_EMEM);

	ptab__mem_enable(p);
}
END_TEST

START_TEST (row_data_s_type)
{
	err = ptab_row_data_s(p, "Row data");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_s(p, "Row data");
	ck_assert_int_eq(err, PTAB_ETYPE);
}
END_TEST

START_TEST (row_data_s_numcolumns)
{
	err = ptab_row_data_s(p, "Row data");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_i(p, "%d", 0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_f(p, "%f", 1.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_s(p, "Row data");
	ck_assert_int_eq(err, PTAB_ECOLUMNS);
}
END_TEST

START_TEST (row_data_i_default)
{
	err = ptab_row_data_i(p, "%d", 5);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (row_data_i_null)
{
	err = ptab_row_data_i(NULL, "%d", 5);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_row_data_i(p, NULL, 5);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (row_data_i_nomem)
{
	ptab__mem_disable(p);

	err = ptab_row_data_i(p, "%d", 5);
	ck_assert_int_eq(err, PTAB_EMEM);

	ptab__mem_enable(p);
}
END_TEST

START_TEST (row_data_i_type)
{
	err = ptab_row_data_i(p, "%d", 5);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_i(p, "%d", 5);
	ck_assert_int_eq(err, PTAB_ETYPE);
}
END_TEST

START_TEST (row_data_i_numcolumns)
{
	err = ptab_row_data_i(p, "%d", 5);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_s(p, "String");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_f(p, "%f", 1.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_i(p, "%d", 4);
	ck_assert_int_eq(err, PTAB_ECOLUMNS);
}
END_TEST

START_TEST (row_data_f_default)
{
	err = ptab_row_data_f(p, "%f", 5.0);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (row_data_f_null)
{
	err = ptab_row_data_f(NULL, "%f", 5.0);
	ck_assert_int_eq(err, PTAB_ENULL);

	err = ptab_row_data_f(p, NULL, 5.0);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (row_data_f_nomem)
{
	ptab__mem_disable(p);

	err = ptab_row_data_f(p, "%f", 5.0);
	ck_assert_int_eq(err, PTAB_EMEM);

	ptab__mem_enable(p);
}
END_TEST

START_TEST (row_data_f_type)
{
	err = ptab_row_data_f(p, "%f", 5.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_f(p, "%f", 5.0);
	ck_assert_int_eq(err, PTAB_ETYPE);
}
END_TEST

START_TEST (row_data_f_numcolumns)
{
	err = ptab_row_data_f(p, "%f", 5.0);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_i(p, "%d", 1);
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_s(p, "String");
	ck_assert_int_eq(err, PTAB_OK);

	err = ptab_row_data_f(p, "%f", 4.0);
	ck_assert_int_eq(err, PTAB_ECOLUMNS);
}
END_TEST

START_TEST (end_row_default)
{
	ptab_row_data_s(p, "String");
	ptab_row_data_i(p, "%d", 5);
	ptab_row_data_f(p, "%f", 3.0);

	err = ptab_end_row(p);
	ck_assert_int_eq(err, PTAB_OK);
}
END_TEST

START_TEST (end_row_null)
{
	err = ptab_end_row(NULL);
	ck_assert_int_eq(err, PTAB_ENULL);
}
END_TEST

START_TEST (end_row_toofew)
{
	ptab_row_data_s(p, "String");
	ptab_row_data_i(p, "%d", 5);

	err = ptab_end_row(p);
	ck_assert_int_eq(err, PTAB_ECOLUMNS);
}
END_TEST

START_TEST (end_row_notbegun)
{
	/*
	 * using a different fixture for this one,
	 * so free the current allocation
	 */
	ptab_free(p);

	fixture_init_columns();

	err = ptab_end_row(p);
	ck_assert_int_eq(err, PTAB_EORDER);
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
	tcase_add_test(tc, begin_row_alreadybegan);

	return tc;
}

TCase *row_data_s_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Row Data (String)");
	tcase_add_checked_fixture(tc, fixture_begin_row_s, fixture_free);
	tcase_add_test(tc, row_data_s_default);
	tcase_add_test(tc, row_data_s_null);
	tcase_add_test(tc, row_data_s_nomem);
	tcase_add_test(tc, row_data_s_type);
	tcase_add_test(tc, row_data_s_numcolumns);

	return tc;
}

TCase *row_data_i_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Row Data (Integer)");
	tcase_add_checked_fixture(tc, fixture_begin_row_i, fixture_free);
	tcase_add_test(tc, row_data_i_default);
	tcase_add_test(tc, row_data_i_null);
	tcase_add_test(tc, row_data_i_nomem);
	tcase_add_test(tc, row_data_i_type);
	tcase_add_test(tc, row_data_i_numcolumns);

	return tc;
}

TCase *row_data_f_test_case(void)
{
	TCase *tc;

	tc = tcase_create("Row Data (Float)");
	tcase_add_checked_fixture(tc, fixture_begin_row_f, fixture_free);
	tcase_add_test(tc, row_data_f_default);
	tcase_add_test(tc, row_data_f_null);
	tcase_add_test(tc, row_data_f_nomem);
	tcase_add_test(tc, row_data_f_type);
	tcase_add_test(tc, row_data_f_numcolumns);

	return tc;
}

TCase *end_row_test_case(void)
{
	TCase *tc;

	tc = tcase_create("End Row");
	tcase_add_checked_fixture(tc, fixture_begin_row_s, fixture_free);
	tcase_add_test(tc, end_row_default);
	tcase_add_test(tc, end_row_null);
	tcase_add_test(tc, end_row_toofew);
	tcase_add_test(tc, end_row_notbegun);

	return tc;
}
