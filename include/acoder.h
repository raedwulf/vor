/**
 * Arithmetic Coder in C
 * based on Shelwien's Range Coder from
 * http://encode.ru/threads/1153-Simple-binary-rangecoder-demo
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

#ifndef _ACODER_H_
#define _ACODER_H_

#include <stdio.h>
#include <stdint.h>

#define AC_SIZE 4
#define AC_BITS (AC_SIZE * 8)
#define AC_SCALE_BITS 15
#define AC_SCALE (1<<AC_SCALE_BITS)
#define AC_SCALE_MASK (AC_SCALE-1)
#define AC_SCALE_HALF (AC_SCALE>>1)

#define AC_STOP 0x01000000U
#define AC_THRESHOLD 0xFF000000U

typedef struct ac_state_s {
	uint32_t range, code, ffnum, cache;
	uint64_t lowc;
	FILE *f;
} ac_state_t;

static inline void ac_init(ac_state_t *s, FILE *f)
{
	s->f = f;
	s->range = 0xFFFFFFFFU;
	s->lowc = 0;
	s->ffnum = 0;
	s->cache = 0;
}

static inline void ac_encoder_init(ac_state_t *s, FILE *f)
{
	ac_init(s, f);
}

static inline void ac_decoder_init(ac_state_t *s, FILE *f)
{
	int i;
	ac_init(s, f);
	for (i = 0; i < AC_SIZE + 1; i++) {
		s->code <<= 8;
		s->code += getc(f);
	}
}

static inline uint32_t ac_shift_low(ac_state_t *s)
{
	uint32_t carry = (uint32_t)(s->lowc >> AC_BITS);
	uint32_t low = (uint32_t)s->lowc;
	if (low < AC_THRESHOLD || carry) {
		putc(s->cache+carry, s->f);
		for (; s->ffnum; s->ffnum--)
			putc(carry - 1, s->f);
		s->cache = low >> (AC_BITS-8);
	} else
		s->ffnum++;
	return s->lowc = (low << 8);
}

static inline void ac_encoder_finish(ac_state_t *s)
{
	int i;
	for (i = 0; i < AC_SIZE + 1; i++)
		ac_shift_low(s);
}

static inline void ac_decoder_finish(ac_state_t *s)
{
}

static inline int ac_encoder_process(ac_state_t *s, uint32_t freq, int bit)
{
	uint32_t rnew = (((uint64_t)s->range) * (freq << (AC_BITS - AC_SCALE_BITS))) >> AC_BITS;
	if (bit) {
		s->range -= rnew;
		s->lowc += rnew;
	} else
		s->range = rnew;
	while (s->range < AC_STOP) {
		s->range <<= 8;
		ac_shift_low(s);
	}
	return bit;
}

static inline int ac_decoder_process(ac_state_t *s, uint32_t freq)
{
	uint32_t rnew = (((uint64_t)s->range) * (freq << (AC_BITS - AC_SCALE_BITS))) >> AC_BITS;
	int bit = (s->code >= rnew);
	if (bit) {
		s->range -= rnew;
		s->code -= rnew;
	} else
		s->range = rnew;
	while (s->range < AC_STOP) {
		s->range <<= 8;
		s->code <<= 8;
		s->code += getc(s->f);
	}
	return bit;
}

#endif
