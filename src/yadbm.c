/**
 * yadbm.c
 *
 * @author: Kamil Żak (楊文里) <defm03@outlook.jp>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * @date: 2013-11-10
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "dbg.h"
#include "yadbm.h"

/** Functions for loading
 * Loading database -> opening file and allocation of whole database into
 * memory with handlers for errors.
 */
int Database_load(struct Connection *conn)
{
    int rc = fread(conn->db, sizeof(struct Database), 1, conn->file);
    check_debug(!rc, "Failed to load database");
    return 0;
error:
    return -1;
}

struct Connection *Database_open(const char *filename, char mode)
{
	struct Connection *conn = malloc(sizeof(struct Connection));
	check_mem(conn);
    check_debug(conn, "Memory error");

	conn->db = malloc(sizeof(struct Database));
	check_mem(conn);
    check_debug(conn->db, "Memory error");

	if(mode == 'c') {
		conn->file = fopen(filename, "w");
	} else {
		conn->file = fopen(filename, "r+");

		if(conn->file) {
			Database_load(conn);
		}
	}

	check_debug(conn->file, "Failed to open file");
error:
   if(conn->db) free(conn->db);
   if(conn->file) fclose(conn->file);
}

/** Functions for closing
 * Closing file, and calls to free memory. Also functions to kill whole
 * process if needed, and sending system errors.
 */

void die(const char *message)
{
	if(errno) {
		perror(message);
	} else {
		printf("ERROR: %s\n", message);
	}
}

void Database_close(struct Connection *conn)
{
	if(conn->file) fclose(conn->file);
	if(conn->db) free(conn->db);
	free(conn);
}

/** Functions for printing
 * Functions to print out content of database.
 */

void Address_print(struct Address *addr)
{
	printf("%d %d %s %s\n",
		addr->id, addr->set, addr-> name, addr->email);
}

void Database_list(struct Connection *conn)
{
	int i = 0;
	struct Database *db = conn->db;
	for (i = 0; i < MAX_ROWS; i++) {
		struct Address *cur = &db->rows[i];

		if(cur->set) {
			Address_print(cur);
		}
	}
}

/** Functions for creating
 * Creating and writing database with error handlers.
 */

int Database_write(struct Connection *conn)
{
	// Sets the position indicator associated with stream to the
	// beginning of the file.
	rewind(conn->file);

	int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
	check_mem(rc);
    check_debug(rc, "Failed to write database");

	// If the given stream was open for writing (or if it was open for
	// updating and the last i/o operation was an output operation) any
	// unwritten data in its output buffer is written to the file.
	rc = fflush(conn->file);
	check_mem(rc);
    check_debug(rc, "Cannot flush database");

    return 0;
error:
    return -1;
}

void Database_create(struct Connection *conn)
{
	int i = 0;

	for (i = 0; i < MAX_ROWS; i++)
	{
		// Make a prototype to initialize it
		struct Address addr = {.id = i, .set = 0};
		// And assigning it
		conn->db->rows[i] = addr;
	}
}

/** Set and get database
 * Setting database rows up - filling them with given data. Error
 * handling included. There may be some problems with strncpy; as you
 * can see - there's small code at the end to add '/0' at the EOF -
 * it should be changed later.
 */

int Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
	struct Address *addr = &conn->db->rows[id];

	// If there is already record on that row, program will end
	if(addr->set) die("Already set, delete it first");
	addr->set = 1;

	char *res = strncpy(addr->name, name, MAX_DATA);
	check_debug(res, "Name copy failed");

	res = strncpy(addr->email, email, MAX_DATA);
	check_debug(res, "Email copy failed");

    /*
	// Adding '/0' at the end of file
	int size = (sizeof(res) / sizeof(char));
	char res[size+1] = "/0";
    */

    return 0;
error:
    if(res) free(res);
    return -1;
}

void Database_get(struct Connection *conn, int id)
{
	struct Address *addr = &conn->db->rows[id];

	if(addr->set) {
		Address_print(addr);
	} else {
		die("ID is not set");
	}
}

/* Delete whole database */
void Database_delete(struct Connection *conn, int id)
{
	struct Address addr = {.id = id, .set = 0};
	conn->db->rows[id] = addr;
}

/** Main loop
 * There is small, silly args parser inside.
 * @USAGE: mudbm <dbfile> <action> [action params]
 */

int main(int argc, char *argv[])
{
	if(argc < 3) die("USAGE: mudbm <dbfile> <action> [action params]");

	char *filename = argv[1];
	char action = argv[2][0];
	struct Connection *conn = Database_open(filename, action);
	int id = 0;

	if(argc > 3) id = atoi(argv[3]);
    if(id >= MAX_ROWS) die("There's not that many records.");

	switch(action) {
		case 'c':
			Database_create(conn);
			Database_write(conn);
			break;
		case 'g':
			if(argc != 4) die("Need id to get");
			Database_get(conn, id);
			break;
		case 's':
			if(argc != 6) die("Need id, name, email to set");
			Database_set(conn, id, argv[4], argv[5]);
			Database_write(conn);
			break;
		case 'd':
			if(argc != 4) die("Need id to delete");
			Database_delete(conn, id);
			Database_write(conn);
			break;
		case 'l':
			Database_list(conn);
			break;
		default:
			die("Invalid action, only: c=create, g=get, s=set, d=del, l=list");
	}

	Database_close(conn);

	return 0;
error:
    return -1;
}
