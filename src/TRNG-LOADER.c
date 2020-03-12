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
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>

#define MAXFILES 100
#define MAXFILENAMELEN 100

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

void find(char **chunkFileNames) {
	DIR *d;
	struct dirent *dirEntry;
	int chunkFileNamesPos = 0;
	d = opendir(".");
	if (d != NULL) {
		while((dirEntry = readdir(d)) != NULL) {
			if(begins(dirEntry->d_name, "CHUNK")) {
				*(chunkFileNames + chunkFileNamesPos) = (char *)malloc(MAXFILENAMELEN * sizeof(char));
				memset(*(chunkFileNames + chunkFileNamesPos), 0, MAXFILENAMELEN * sizeof(char));
				strcpy(*(chunkFileNames + chunkFileNamesPos), dirEntry->d_name);
				chunkFileNamesPos++;
			}
		}
		closedir(d);
	}
}


void run(void) {
	char **chunkFileNames = (char **)malloc(MAXFILES * sizeof(char *));
	memset(chunkFileNames, 0, MAXFILES * sizeof(char *));

	find(chunkFileNames);

	for(int i = 0; i < MAXFILES; i++) {
		if(*(chunkFileNames + i) != NULL) {
			printf("FILE %02d: %s\n", i, *(chunkFileNames + i));
		} else {
			break;
		}
	}


	free(chunkFileNames);
}

int main(void) {


	run();
/*
	MYSQL *con = mysql_init(NULL);

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
*/
}

