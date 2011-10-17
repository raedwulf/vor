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

long flen(FILE* f)
{
	long len;
	fseek(f, 0, SEEK_END);
	len = ftell(f);
	fseek(f, 0, SEEK_SET);
	return len;
}

int main(int argc, char** argv)
{
	int i;
	long f_len;
	FILE *f, *g;
	ac_state_t s;
	pd_order0_t p;

	if (argc<4)
		return 1;
	if (!(f = fopen(argv[2], "rb")))
		return 2;
	if (!(g = fopen(argv[3], "wb")))
		return 3;

	pd_order0_init(&p);
	if (argv[1][0]=='c') {
		f_len = flen(f);
		fwrite(&f_len, 1, 4, g);
		ac_encoder_init(&s, g);

		for (i = 0; i < f_len; i++) {
			int j;
			int c = getc(f);
			pd_order0_byte(&p);
			/*for (j = 0x80; j > 0; j >>= 1)*/
				/*pd_order0_update(&p,*/
						 /*ac_encoder_process(&s, pd_order0_probability(&p), c & j));*/
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x80));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x40));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x20));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x10));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x08));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x04));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x02));
			pd_order0_update(&p,
					 ac_encoder_process(&s, pd_order0_probability(&p), c & 0x01));
		}

		ac_encoder_finish(&s);
	} else {
		fread(&f_len, 1, 4, f);
		ac_decoder_init(&s, f);

		for (i = 0; i < f_len; i++) {
			int j;
			for (j = 0; j < 8; j++)
				pd_order0_update(&p,
						 ac_decoder_process(&s, pd_order0_probability(&p)));
			putc(pd_order0_byte(&p), g);
		}

		ac_decoder_finish(&s);
	}
	fclose(f);
	fclose(g);
	return 0;
}
