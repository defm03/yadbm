/**
 * yadbm.h
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

#ifndef __yadbm_h__
#define __yadbm_h__

#define MAX_DATA 512
#define MAX_ROWS 100

// Basic structs to work with.
struct Address {
	int id;
	int set;
	char name[MAX_DATA];
	char email[MAX_DATA];
};

struct Database {
	struct Address rows[MAX_ROWS];
};

struct Connection {
	FILE *file;
	struct Database *db;
};

// Extern for functions to prevent errors
extern int Database_load(struct Connection *conn);
extern void Database_delete(struct Connection *conn, int id);
extern void Database_get(struct Connection *conn, int id);
extern int Database_set(struct Connection *conn, int id, const char *name, const char *email);
extern void Database_create(struct Connection *conn);
extern int Database_write(struct Connection *conn);
extern void Database_list(struct Connection *conn);
extern void Address_print(struct Address *addr);
extern void Database_close(struct Connection *conn);
extern void die(const char *message);

#endif
