/*
 *
 * True Random Number Generator (TRNG) Server
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wiringPi.h>

#define CHUNKSIZE 128 /* BYTES */
#define FILESIZE  100 /* CHUNKS */
#define MAXFILENAMELEN 500
#define CLOCK 0
#define DATA 2
#define POWER 3

#define SETBIT(x, i) x |= (1 << i)
#define RSTBIT(x, i) x &= ~(1 << i)

char *chunkDirectory;
int chunkFileCount;
void read(int *, int);
void build(unsigned char *, int *, int);
void run();
void init(char *);
void powerOn();
void powerOff();

void read(int *dataBuffer, int dataBufferSize) {
	int dataBufferPos = 0;
	int clock = 0;
	int lastClock = 0;
	do {
		clock = digitalRead(CLOCK);
		if(clock != lastClock) {
			*(dataBuffer + dataBufferPos) = digitalRead(DATA);
			lastClock = clock;
			dataBufferPos++;
		}
	} while (dataBufferPos < dataBufferSize);
}

void build(unsigned char *chunkBuffer, int *dataBuffer, int chunkBufferSize) {
	int chunkBufferPos = 0;
	int dataBufferPos = 0;
	do {
		for(int bit = 7; bit > -1; bit--) {
			if(*(dataBuffer + dataBufferPos++) == 1) {
				SETBIT(*(chunkBuffer + chunkBufferPos), bit);
			} else {
				RSTBIT(*(chunkBuffer + chunkBufferPos), bit);
			}
		}
		chunkBufferPos++;
	} while (chunkBufferPos < chunkBufferSize);
}

void write(void) {
	time_t t;
	struct tm localTime;
	FILE *chunkFile;
	char *chunkFileName = (char *) malloc(MAXFILENAMELEN * sizeof(char));
        int *dataBuffer = (int *) malloc(CHUNKSIZE * sizeof(int) * 8);
        unsigned char *chunkBuffer = (unsigned char *) malloc(CHUNKSIZE * sizeof(unsigned char));
	memset(chunkFileName, 0, MAXFILENAMELEN * sizeof(char));
	time(&t);
	localTime = *localtime(&t);
	sprintf(chunkFileName, "%s/CHUNK-%04d%02d%02d%02d%02d.txt", chunkDirectory, localTime.tm_year + 1900, localTime.tm_mon + 1, localTime.tm_mday, localTime.tm_hour, localTime.tm_min);
	fprintf(stdout, "TRNG-CHUNKER: Writing chunk file %s.\n", chunkFileName);
	for(int c = 0; c < FILESIZE; c++) {
	        memset(dataBuffer, 0, CHUNKSIZE * sizeof(int) * 8);
        	memset(chunkBuffer, 0, CHUNKSIZE * sizeof(char));
	        read(dataBuffer, CHUNKSIZE * 8);
	        build(chunkBuffer, dataBuffer, CHUNKSIZE);
		chunkFile = fopen(chunkFileName, "a");
		for(int i = 0; i < CHUNKSIZE; i++) {
	                fprintf(chunkFile, "%02X", *(chunkBuffer + i));
	        }
		fprintf(chunkFile, "\n");
	        fclose(chunkFile);
	}
	free(dataBuffer);
	free(chunkBuffer);
}

void run() {
	for(int i = 0; i < chunkFileCount; i++) {
		write();
	}
}

void init(char *arg) {
        chunkDirectory = (char *)malloc(MAXFILENAMELEN * sizeof(char));
        memset(chunkDirectory, 0, MAXFILENAMELEN * sizeof(char));
	strcpy(chunkDirectory, arg);
	fprintf(stdout, "True Random Number Generator Server (Chunker)\n");
	fprintf(stdout, "(C)Copyright 2020 Manny Peterson\n\n");
	fprintf(stdout, "Files: %d\n", chunkFileCount);
	fprintf(stdout, "Using: %s\n", chunkDirectory);
	wiringPiSetup();
        pinMode(CLOCK, INPUT);
        pinMode(DATA, INPUT);
	pinMode(POWER, OUTPUT);
}

void powerOn(void) {
	digitalWrite(POWER, HIGH);
	fprintf(stdout, "TRNG-CHUNKER: Powering on TRNG board.\n");
	delay(5000);
}

void powerOff(void) {
	digitalWrite(POWER, LOW);
	fprintf(stdout, "TRNG-CHUNKER: Powering off TRNG board.\n");
	delay(5000);
}

int main(int argc, char *argv[]) {
	if(argc != 3) {
		fprintf(stdout, "TRNG-CHUNKER: Missing parameter chunk directory and/or chunk file count.\n");
		exit(1);
	}
	chunkFileCount = atoi(argv[2]);
	if(chunkFileCount < 1) {
		fprintf(stdout, "TRNG-CHUNKER: Must specify a chunk file count of 1 or greater.\n");
		exit(1);
	}
	init(argv[1]);
	powerOn();
	run();
	powerOff();
	exit(0);
}
