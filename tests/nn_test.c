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
#define MAX_CONTEXTS 4
#include "mix_nn0.h"

int main(int argc, char **argv)
{
	mix_nn0_t nn;
	mix_nn0_init(&nn);

	float result[8];
	for (int i = 0; i < sizeof(result)/sizeof(float); i++) {
		uint16_t contexts[MAX_CONTEXTS];
		contexts[0] = (i & 1) * UINT16_MAX;
		contexts[1] = (i * UINT16_MAX) / sizeof(result)/sizeof(float);
		result[i] = mix_nn0_mix(&nn, contexts);
		mix_nn0_update(&nn, i);
	}
	return 0;
}
