/**
 * Adaptive Arithmetic Coder
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
#include <stdint.h>
#include "acoder.h"
#include "pd_order0.h"

#define AC_TEST_BUFFER_SIZE (1024 * 64)
static uint8_t buffer[AC_TEST_BUFFER_SIZE];

static long flen(FILE* f)
{
	long len;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	return len;
}

static void compress(FILE *f, FILE *g)
{
	long f_len = flen(f);
	fwrite(&f_len, 1, sizeof(long), g);

	ac_state_t s;
	ac_encoder_init(&s, g);

	pd_order0_t p;
	pd_order0_init(&p);

	while (!feof(f)) {
		int bytes = fread(buffer, 1, AC_TEST_BUFFER_SIZE, f);
		for (int i = 0; i < bytes; i++) {
			pd_order0_reset(&p);
			for (uint8_t j = 0x80; j; j >>= 1) {
				int a = ac_encoder_process(&s, pd_order0_probability(&p), buffer[i] & j);
				pd_order0_update(&p, a);
			}
		}
	}

	ac_encoder_finish(&s);
}

static inline void decompress_block(FILE *f, FILE *g, ac_state_t *s, pd_order0_t *p, int size) {
	for (int k = 0; k < size; k++) {
		for (int j = 0; j < 8; j++) {
			int a = ac_decoder_process(s, pd_order0_probability(p));
			pd_order0_update(p, a);
		}
		buffer[k] = pd_order0_reset(p);
	}
	fwrite(buffer, 1, size, g);
}

static void decompress(FILE *f, FILE *g)
{
	long f_len;
	fread(&f_len, 1, sizeof(long), f);

	ac_state_t s;
	ac_decoder_init(&s, f);

	pd_order0_t p;
	pd_order0_init(&p);

	long l = (f_len - (f_len % AC_TEST_BUFFER_SIZE)) / AC_TEST_BUFFER_SIZE;
	for (int i = 0; i < l; i++)
		decompress_block(f, g, &s, &p, AC_TEST_BUFFER_SIZE);

	decompress_block(f, g, &s, &p, f_len % AC_TEST_BUFFER_SIZE);

	ac_decoder_finish(&s);
}

int main(int argc, char** argv)
{
	FILE *f, *g;

	if (argc<4)
		return 1;
	if (!(f = fopen(argv[2], "rb")))
		return 2;
	if (!(g = fopen(argv[3], "wb")))
		return 3;

	if (argv[1][0]=='c')
		compress(f, g);
	else
		decompress(f, g);

	fclose(f);
	fclose(g);
	return 0;
}
