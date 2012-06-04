/**
 * Vor main program
 *
 * Copyright (C) 2011 by Tai Chi Minh Ralph Eastwood
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_FILES 1024

static int verbose_flag;    /* Flag set by ‘--verbose’. */
static int stdout_flag;     /* Flag set by ‘--stdout’. */
static int compress_flag;   /* Flag set by ‘--compress’. */
static int decompress_flag; /* Flag set by ‘--decompress’. */
static int compress_level;  /* Compression level */
static char *input_files[MAX_FILES]; /* First argument */
static int num_files = 0;

int main (int argc, char **argv)
{
	verbose_flag = 1;
	stdout_flag = 0;
	compress_flag = 0;
	decompress_flag = 0;
	compress_level = 4;
	num_files = 0;
	memset(input_files, 0, sizeof(input_files));
	while (1) {
		static struct option long_options[] = {
			{"verbose", no_argument, &verbose_flag, 2},
			{"quiet", no_argument, &verbose_flag, 0},
			{"stdout", no_argument, &stdout_flag, 1},
			{"compress", no_argument, &compress_flag, 1},
			{"decompress", no_argument, &decompress_flag, 1},
			{"fast", no_argument, &compress_level, 1},
			{"best", no_argument, &compress_level, 9},
			{"help", no_argument, &stdout_flag, 'h'},
			{0, 0, 0, 0}
		};
		/* getopt_long stores the option index here. */
		int option_index = 0;
		int c = getopt_long (argc, argv, "vczdh",
			long_options, &option_index);
		/* detect the end of the options. */
		if (c == -1)
			break;
		switch (c) {
		case 'v': verbose_flag = 1; break;
		case 'c': stdout_flag = 1; break;
		case 'z': compress_flag = 1; break;
		case 'd': decompress_flag = 1; break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			compress_level = c - '0'; break;
		case 'h':
			printf("Usage: vor [OPTIONS]... [FILES]...\n");
			printf("Compress or decompress files (by default compress in-place).\n\n");
			printf("Default is to use STDIN if no file is specified or if - is given.\n");
			exit(0);
		default:
			abort();
		}
        }

	/* rest of arguments are filenames */
	if (optind < argc) {
		char *filename;
		while (optind < argc) {
			struct stat s;
			filename = argv[optind++];
			/* do some sanity checking to make sure that the files can be accessed */
			if (stat(filename, &s)) {
				if (S_ISDIR(s.st_mode)) {
					fprintf(stderr, "ERROR: %s is a directory.", filename);
					abort();
				}
			} else {
				fprintf(stderr, "ERROR: %s does not exist.", filename);
				abort();
			}
			input_files[num_files++] = filename;
		}
        }
	exit(0);
}
