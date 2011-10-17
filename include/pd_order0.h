/**
 * Order-0 Predictor in C
 * based on Shelwien's predictor from
 * http://encode.ru/threads/1153-Simple-binary-rangecoder-demo
 *
 * Written by Tai Chi Minh Ralph Eastwood
 * Put in Public Domain (for countries that this is legally possible).
 *
 * Otherwise, this code is licensed using the MIT license:
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

#ifndef _PD_ORDER0_H_
#define _PD_ORDER0_H_

#include <stdint.h>
#include "acoder.h"

#define SIZEOF_P 255

typedef struct pd_order0_data_s {
	uint8_t cxt;
	uint16_t p[SIZEOF_P];
} pd_order0_t;

static inline uint32_t pd_order0_byte(pd_order0_t *d)
{
	uint32_t c = d->cxt;
	d->cxt = 1;
	return c;
}

static inline void pd_order0_init(pd_order0_t *d)
{
	int i;
	for (i = 0; i < sizeof(d->p) / sizeof(d->p[0]); i++)
		d->p[i] = AC_SCALE_HALF;
	pd_order0_byte(d);
}
static inline uint32_t pd_order0_probability(pd_order0_t *d)
{
	return d->p[d->cxt-1];
}

static inline void pd_order0_update(pd_order0_t *d, int y)
{
	if (y) {
		d->p[d->cxt-1] -= d->p[d->cxt-1] >> 5;
		d->cxt <<= 1;
		d->cxt++;
	} else {
		d->p[d->cxt-1] += (AC_SCALE - d->p[d->cxt-1]) >> 5;
		d->cxt <<= 1;
	}
}

#endif
