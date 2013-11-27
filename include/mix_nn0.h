/**
 * Neural-network mixer
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

#ifndef _PD_MIXER_NN0_H_
#define _PD_MIXER_NN0_H_

#include <stdint.h>
#include <math.h>

#ifndef MAX_CONTEXTS
#error "MAX_CONTEXTS must be defined."
#endif
#ifndef NN0_LEARNING_RATE
#define NN0_LEARNING_RATE 0.01f
#endif
#ifndef NN0_EPSILON
#define NN0_EPSILON 0.0001f
#endif

typedef struct {
	float weights[MAX_CONTEXTS];
	float x[MAX_CONTEXTS];
	float m;
} mix_nn0_t;

static inline void mix_nn0_init(mix_nn0_t *nn)
{
	int i;
	for (i = 0; i < MAX_CONTEXTS; i++)
		nn->weights[i] = 0.5f;
}

static inline float mix_nn0_mix(mix_nn0_t *nn, uint16_t p[MAX_CONTEXTS])
{
	int i;
	nn->m = 0.0f;
	for (i = 0; i < MAX_CONTEXTS; i++) {
		float pi = (float)p[i] / (float)UINT16_MAX;
		if (pi == 0.0f) pi = NN0_EPSILON;
		if (pi == 1.0f) pi = 1.0f - NN0_EPSILON;
		nn->x[i] = log(pi/(1.0f-pi));
	}
	for (i = 0; i < MAX_CONTEXTS; i++)
		nn->m += nn->weights[i] * nn->x[i];
	nn->m = 1.0f / (1.0f + exp(-nn->m));
	return nn->m;
}

static inline void mix_nn0_update(mix_nn0_t *nn, int y)
{
	int i;
	for (i = 0; i < MAX_CONTEXTS; i++)
		nn->weights[i] += NN0_LEARNING_RATE * nn->x[i] * (y - nn->m);
}
#endif
