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

#define CHUNKSIZE 128 /* BYTES */
#define FILESIZE 100 /* CHUNKS */
#define MAXFILES 100
#define MAXFILENAMELEN 100

int begins(char *, char *);
void find(char **);
void load(char *, char **);
void run(void);

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
			if(begins(dirEntry->d_name, "CHUNK-")) {
				*(chunkFileNames + chunkFileNamesPos) = (char *)malloc(MAXFILENAMELEN * sizeof(char));
				memset(*(chunkFileNames + chunkFileNamesPos), 0, MAXFILENAMELEN * sizeof(char));
				strcpy(*(chunkFileNames + chunkFileNamesPos), dirEntry->d_name);
				chunkFileNamesPos++;
			}
		}
		closedir(d);
	}
}

void load(char *chunkFileName,  char **chunksBuffer) {
	FILE *chunkFile;
	int chunksBufferPos = 0;
	char *chunkBuffer = (char *)malloc(CHUNKSIZE * 2 * sizeof(char) + 10); /* ADDING 10 BYTES TO ACCOMODATE NEWLINE, ETC. */
	memset(chunkBuffer, 0, CHUNKSIZE * 2 * sizeof(char) + 10);
	chunkFile = fopen(chunkFileName, "r");
	while(fgets(chunkBuffer, CHUNKSIZE * 2 *  sizeof(char) + 10, chunkFile) != NULL) {
		*(chunksBuffer + chunksBufferPos) = (char *)malloc(CHUNKSIZE * 2 * sizeof(char) + 10);
		memset(*(chunksBuffer + chunksBufferPos), 0, CHUNKSIZE * 2 * sizeof(char) + 10);
		memcpy(*(chunksBuffer + chunksBufferPos), chunkBuffer, CHUNKSIZE * 2 * sizeof(char));
		chunksBufferPos++;
	}
	free(chunkBuffer);
	fclose(chunkFile);
}

void insert(char **chunksBuffer) {
        MYSQL *db = mysql_init(NULL);
	char *query = (char *)malloc(500 * sizeof(char));
	memset(query, 0, 500 * sizeof(char));

        if(db == NULL) {
                fprintf(stderr, "%s\n", mysql_error(db));
                return;
        }
        if(mysql_real_connect(db, "localhost", "TRNG", "TRNGTRNG", "TRNG", 0, NULL, 0) == NULL) {
                fprintf(stderr, "%s\n", mysql_error(db));
                return;
        }
	for(int i = 0; i < FILESIZE; i++) {
		if(*(chunksBuffer + i) != NULL) {
			printf("CHUNK %03d: %s\n", i, *(chunksBuffer + i));
			memset(query, 0, 500 * sizeof(char));
			strcat(query, "INSERT INTO chunks VALUES ('");
			strcat(query, *(chunksBuffer + i));
			strcat(query, "', NOW())");
			mysql_query(db, query);
			free(*(chunksBuffer + i));
		} else {
			break;
		}
	}
        mysql_close(db);
}

void run(void) {
	char **chunkFileNames = (char **)malloc(MAXFILES * sizeof(char *));
	memset(chunkFileNames, 0, MAXFILES * sizeof(char *));
	char **chunksBuffer = (char **)malloc(FILESIZE * sizeof(char *));
	memset(chunksBuffer, 0, FILESIZE * sizeof(char *));
	find(chunkFileNames);
	for(int i = 0; i < MAXFILES; i++) {
		if(*(chunkFileNames + i) != NULL) {
			printf("FILE %02d: %s\n", i, *(chunkFileNames + i));
			load(*(chunkFileNames + i), chunksBuffer);
			insert(chunksBuffer);
			free(*(chunkFileNames + i)); // REMEMBER TO DO THIS LAST
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

