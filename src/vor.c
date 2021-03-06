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
#include <assert.h>

/**
 * Prepare the context models.
 */
#include "acoder.h"
#include "pd_order0.h"
#include "pd_order1.h"
#define MAX_CONTEXTS 2
#include "mix_nn0.h"

#define MAX_FILES 1024

static int verbose_flag;    /* flag set by ‘--verbose’. */
static int stdout_flag;     /* flag set by ‘--stdout’. */
static int compress_flag;   /* flag set by ‘--compress’. */
static int decompress_flag; /* flag set by ‘--decompress’. */
static int compress_level;  /* compression level */
static char *input_files[MAX_FILES]; /* first argument */
static int num_files = 0;

typedef enum {
	VOR_COMPRESS_MODE,
	VOR_DECOMPRESS_MODE
} vor_mode_t;
typedef struct {
	vor_mode_t m;
	ac_state_t s;
	pd_order0_t p0;
	pd_order1_t p1;
	mix_nn0_t nn;
	uint64_t length;
} vor_t;

void vor_solid_init(vor_t *v, vor_mode_t m, FILE *stream, uint64_t length)
{
	pd_order0_init(&v->p0);
	pd_order1_init(&v->p1);
	mix_nn0_init(&v->nn);
	v->m = m;
	v->length = length;
	switch (m) {
		case VOR_COMPRESS_MODE:
			fwrite(&length, 1, sizeof(uint64_t), stream);
			ac_encoder_init(&v->s, stream);
			break;
		case VOR_DECOMPRESS_MODE:
			fread(&v->length, 1, sizeof(uint64_t), stream);
			ac_decoder_init(&v->s, stream);
			break;
	}
}

size_t vor_write(vor_t *v, const void *ptr, size_t size, size_t count)
{
	assert(v->m == VOR_COMPRESS_MODE);
	size_t bytes = size * count;
	const char *p = (const char *)ptr;
	while (bytes--) {
		int c = *p++;
		pd_order0_reset(&v->p0);
		pd_order1_reset(&v->p1);
		for (int j = 0x80; j > 0; j >>= 1) {
			uint16_t p[MAX_CONTEXTS];
			p[0] = pd_order0_probability(&v->p0);
			p[1] = pd_order1_probability(&v->p1);
			float m = mix_nn0_mix(&v->nn, p);
			int a = ac_encoder_process(&v->s, m > 0.5, c & j);
			pd_order0_update(&v->p0, a);
			pd_order1_update(&v->p1, a);
			mix_nn0_update(&v->nn, a);
		}
	}
	return size * count;
}

size_t vor_read(vor_t *v, void *ptr, size_t size, size_t count)
{
	assert(v->m == VOR_DECOMPRESS_MODE);
	size_t bytes = size * count;
	char *p = ptr;
	while (bytes--) {
		for (int j = 0x80; j > 0; j >>= 1) {
			uint16_t p[MAX_CONTEXTS];
			p[0] = pd_order0_probability(&v->p0);
			p[1] = pd_order1_probability(&v->p1);
			float m = mix_nn0_mix(&v->nn, p);
			int a = ac_decoder_process(&v->s, m > 0.5);
			pd_order0_update(&v->p0, a);
			pd_order1_update(&v->p1, a);
			mix_nn0_update(&v->nn, a);
		}
		*p++ = pd_order0_reset(&v->p0);
		pd_order1_reset(&v->p1);
	}
	return size * count;
}

void vor_flush(vor_t *v)
{
	switch (v->m) {
		case VOR_COMPRESS_MODE:	  ac_encoder_finish(&v->s); break;
		case VOR_DECOMPRESS_MODE: ac_decoder_finish(&v->s); break;
	}
}

int main(int argc, char **argv)
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
			printf("Compress or decompress files (by default "
					"compress in-place).\n\n");
			printf("Default is to use STDIN if no file is"
					"specified or if - is given.\n");
			return 0;
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
			/* do some sanity checking to make sure that the
			 * files can be accessed */
			if (stat(filename, &s)) {
				if (S_ISDIR(s.st_mode)) {
					fprintf(stderr,
						"ERROR: %s is a directory.\n",
						filename);
					abort();
				}
			} else {
				fprintf(stderr, "ERROR: %s does not exist.\n",
					filename);
				abort();
			}
			input_files[num_files++] = filename;
		}
        } else {
		input_files[num_files++] = "-";
	}

	if (compress_flag && decompress_flag) {
		fprintf(stderr, "ERROR: cannot compress and decompress at"
				"the same time.\n");
	}

	if (!compress_flag && !decompress_flag) {
		fprintf(stderr, "ERROR: nothing to do.\n");
	}

	int i;
	for (i = 0; i < num_files; i++) {
		if (compress_flag) {
		} else {
		}
	}
	return 0;
}
