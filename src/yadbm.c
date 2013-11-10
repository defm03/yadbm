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

#define MAX_DATA 512
#define MAX_ROWS 100

// Basic structs to work with.
struct Address {
	int id;
	int set;
	char name[MAX_DATA];
	char email[MAX_DATA]
};

struct Database {
	struct Address rows[MAX_ROWS];
};

struct Connection
{
	FILE *file;
	struct Database *db;
};

/** Functions for loading
 * Loading database -> opening file and allocation of whole database into  
 * memory with handlers for errors. 
 */
void Database_load(struct Connection *conn)
{
	int rc = fread(conn->db, sizeof(struct Database), 1, conn->file)
	if(!rc) die("Failed to load database")
}

struct Connection *Database_open(const char *filename, char mode)
{
	struct Connection *conn = malloc(sizeof(struct Connection));
	if(!conn) die("Memory error");

	conn->db = malloc(sizeof(struct Database));
	if(!conn->db) die("Memory error");

	if(mode == 'c') {
		conn->file = fopen(filename, "w");
	} else {
		conn->file = fopen(filename, "r+");

		if(conn->file) {
			Database_load(conn);
		}
	}

	if(!conn->file) die("Failed to open file");
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
	for (int i = 0; i < MAX_ROWS; i++) {
		struct Address *cur = &db->rows[i];

		if(cur->set) {
			Address_print(cur);
		}
	}
}

/** Functions for creating
 * Creating and writing database with error handlers. 
 */

void Database_write(struct Connection *conn)
{
	// Sets the position indicator associated with stream to the 
	// beginning of the file.
	rewind(conn->file);

	int rc = fwrite(conn->db, sizeof(struct Database), 1, conn->file);
	if(rc != 1) die("Failed to write database");

	// If the given stream was open for writing (or if it was open for 
	// updating and the last i/o operation was an output operation) any 
	// unwritten data in its output buffer is written to the file.
	rc = fflush(conn->file); 
	if(rc != 1) die("Cannot flush database");
}

void Database_create(struct Connection *conn)
{
	int i = 0;

	for (int i = 0; i < MAX_ROWS; i++)
	{
		// Make a prototype to initialize it 
		struct Address addr = {.id = i .set = 0};
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

void Database_set(struct Connection *conn, int id, const char *name, const char *email)
{
	struct Address *addr = &conn->db->rows[id];
	
	// If there is already record on that row, program will end 
	if(addr->set) die("Already set, delete it first");
	addr->set = 1;

	char *res = strncpy(addr->name, name, MAX_DATA);
	if(!res) die("Name copy failed");

	res = strncpy(addr->email, email, MAX_DATA);
	if(!res) die("Email copy failed");

	// Adding '/0' at the end of file
	int size = (sizeof(res) / sizeof(char);
	char res[size+1] = '/0';
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

int main(int argc, char const *argv[])
{
	if(argc < 3) die("USAGE: mudbm <dbfile> <action> [action params]");

	char *filename = argv[1];
	char action = argv[2][0];
	struct Connection *conn = Database_open(filename, action);
	int id = 0;

	if(argc > 3) id = atoi(argv[3]);
	if(id >= MAX_ROWS) die ("There's not that many records.")

	return 0;
}