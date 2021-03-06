
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ptab.h>

int main(void)
{
	ptab_t *table;
	const char *libversion, *myversion;

	libversion = ptab_version();
	myversion = PTAB_VERSION;
	if (libversion[0] != myversion[0]) {
		fprintf(stderr, "libptab version mismatch:\n");
		fprintf(stderr, "\tcompiled against: %s\n", myversion);
		fprintf(stderr, "\tlinked with:      %s\n", libversion);
		return EXIT_FAILURE;
	}

	table = ptab_init(NULL);
	if (table == NULL) {
		fprintf(stderr, "error: ptab_init: memory allocation failed\n");
		return EXIT_FAILURE;
	}

	/* NFC east standings as of 2014-12-06 */

	ptab_column(table, "Team", PTAB_STRING);
	ptab_column(table, "Wins", PTAB_INTEGER);
	ptab_column(table, "Losses", PTAB_INTEGER);
	ptab_column(table, "Percent", PTAB_FLOAT);

	ptab_begin_row(table);
	ptab_row_data_s(table, "Philadelphia");
	ptab_row_data_i(table, "%d", 9);
	ptab_row_data_i(table, "%d", 3);
	ptab_row_data_f(table, "%0.3f", 9.0 / 12.0);
	ptab_end_row(table);

	ptab_begin_row(table);
	ptab_row_data_s(table, "Dallas");
	ptab_row_data_i(table, "%d", 9);
	ptab_row_data_i(table, "%d", 4);
	ptab_row_data_f(table, "%0.3f", 9.0 / 13.0);
	ptab_end_row(table);

	ptab_begin_row(table);
	ptab_row_data_s(table, "New York");
	ptab_row_data_i(table, "%d", 3);
	ptab_row_data_i(table, "%d", 9);
	ptab_row_data_f(table, "%0.3f", 3.0 / 12.0);
	ptab_end_row(table);

	ptab_begin_row(table);
	ptab_row_data_s(table, "Washington");
	ptab_row_data_i(table, "%d", 3);
	ptab_row_data_i(table, "%d", 9);
	ptab_row_data_f(table, "%0.3f", 3.0 / 12.0);
	ptab_end_row(table);

	ptab_dumpf(table, stdout, PTAB_ASCII);

	ptab_free(table);

	return EXIT_SUCCESS;
}
