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
#define MAXFILENAMELEN 500
#define MAXQUERYLEN 500
#define MAX

char *chunkDirectory;
int begins(char *, char *);
void find(char **);
void load(char *, char **);
void run(void);
void init(char *);

int begins(char *str, char *pre) {
	size_t strLen = strlen(str);
	size_t preLen = strlen(pre);
	if(strLen < preLen) {
		return(0);
	} else if(memcmp(pre, str, preLen) == 0) {
		return(1);
	} else {
		return(0);
	}
}

void find(char **chunkFileNames) {
	DIR *d;
	struct dirent *dirEntry;
	int chunkFileNamesPos = 0;
	d = opendir(chunkDirectory);
	if (d != NULL) {
		while((dirEntry = readdir(d)) != NULL) {
			if(begins(dirEntry->d_name, "CHUNK-")) {
				*(chunkFileNames + chunkFileNamesPos) = (char *)malloc(MAXFILENAMELEN * sizeof(char));
				memset(*(chunkFileNames + chunkFileNamesPos), 0, MAXFILENAMELEN * sizeof(char));
				strcpy(*(chunkFileNames + chunkFileNamesPos), chunkDirectory);
				strcat(*(chunkFileNames + chunkFileNamesPos), "/");
				strcat(*(chunkFileNames + chunkFileNamesPos), dirEntry->d_name);
				chunkFileNamesPos++;
			}
		}
		closedir(d);
	}
}

void load(char *chunkFileName,  char **chunksBuffer) {
	FILE *chunkFile;
	int chunksBufferPos = 0;
	char *chunkBuffer = (char *)malloc(CHUNKSIZE * 2 * sizeof(char) + 10); /* ADDING 10 BYTES TO ACCOMODATE NEWLINE, NULL, ETC. */
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
	char *query = (char *)malloc(MAXQUERYLEN * sizeof(char));
	memset(query, 0, MAXQUERYLEN * sizeof(char));

        if(db == NULL) {
                fprintf(stdout, "TRNG-LOADER: %s\n", mysql_error(db));
                exit(1);
        }
        if(mysql_real_connect(db, "localhost", "TRNG", "TRNGTRNG", "TRNG", 0, NULL, 0) == NULL) {
                fprintf(stdout, "TRNG-LOADER: %s\n", mysql_error(db));
                exit(1);
        }
	for(int i = 0; i < FILESIZE; i++) {
		if(*(chunksBuffer + i) != NULL) {
			memset(query, 0, MAXQUERYLEN * sizeof(char));
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
	find(chunkFileNames);
	for(int i = 0; i < MAXFILES; i++) {
		if(*(chunkFileNames + i) != NULL) {
			fprintf(stdout, "TRNG-LOADER: Processing chunk file %s\n", *(chunkFileNames + i));
			memset(chunksBuffer, 0, FILESIZE * sizeof(char *));
			load(*(chunkFileNames + i), chunksBuffer);
			insert(chunksBuffer);
			free(*(chunkFileNames + i)); // REMEMBER TO DO THIS LAST
		} else {
			break;
		}
	}
	free(chunksBuffer);
	free(chunkFileNames);
}

void init(char *arg) {
        chunkDirectory = (char *)malloc(MAXFILENAMELEN * sizeof(char));
        memset(chunkDirectory, 0, MAXFILENAMELEN * sizeof(char));
        strcpy(chunkDirectory, arg);
        fprintf(stdout, "True Random Number Generator Server (Loader)\n");
        fprintf(stdout, "(C)Copyright 2020 Manny Peterson\n\n");
        fprintf(stdout, "Using: %s\n", chunkDirectory);
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stdout, "TRNG-LOADER: Missing parameter chunk directory.\n");
		exit(1);
	}
	init(argv[1]);
	run();
	exit(0);
}

