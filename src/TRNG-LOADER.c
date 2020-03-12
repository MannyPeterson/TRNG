/*
 *
 * True Random Number Generator (TRNG)
 * Copyright (C) 2020 Manny Peterson <me@mannypeterson.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include <mysql.h>
#include <stddef.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int begins(char *str, char *pre) {
	size_t strLen = strlen(str);
	size_t preLen = strlen(pre);
	if(strLen < preLen) {
		return 0;
	} else if(memcmp(pre, str, preLen) == 0) {
		return 1;
	} else {
		return 0;
	}
}

void list(void) {
	DIR *d;
	struct dirent *dirEntry;

	d = opendir(".");
	if (d != NULL) {
		while((dirEntry = readdir(d)) != NULL) {
			if(begins(dirEntry->d_name, "CHUNK")) {
				printf("%s <-- BEGINS WITH TRNG\n", dirEntry->d_name);
			} else {
				printf("%s\n", dirEntry->d_name);
			}
		}
		closedir(d);
	}
}


int main(void) {
	MYSQL *con = mysql_init(NULL);

	list();

	if(con == NULL) {
		fprintf(stderr, "%s\n", mysql_error(con));
		return(1);
	}

	if(mysql_real_connect(con, "localhost", "TRNG", "TRNGTRNG", "TRNG", 0, NULL, 0) == NULL) {
                fprintf(stderr, "%s\n", mysql_error(con));
                return(1);
	}

	mysql_query(con, "INSERT INTO chunks VALUES ('ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWX', NOW());");


	mysql_close(con);

}

