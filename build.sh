#!/bin/sh
gcc -o ./bin/TRNG-CHUNKER ./src/TRNG-CHUNKER.c -lwiringPi
gcc -o ./bin/TRNG-LOADER ./src/TRNG-LOADER.c $(mariadb_config --cflags --libs)
