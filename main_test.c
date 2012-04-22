/*
 * main_test.c
 * This file is part of AudioFader 
 *
 * Copyright (C) 2012 - Timothy
 *
 * AudioFader is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * AudioFader is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>
#include <stdlib.h>
#include "audio_fader.h"

#define ERR_OPEN -1
#define ERR_READ -2
#define ERR_WRITE -3

#define BUF_SIZE 44100
#define FILE_NAME_IN "sin-f1000.pcm"
#define FILE_NAME_OUT "sin-f1000-fade.pcm"

int pcm_buf[BUF_SIZE];

int main()
{
	FILE *pcm = fopen(FILE_NAME_IN, "r" );
	if (pcm == 0) {
		fprintf(stderr, "Failed to open file: %s\n", FILE_NAME_IN);
		exit(ERR_OPEN);
	}

	int r = fread((void *)pcm_buf, sizeof(int), BUF_SIZE, pcm);
	if (r != BUF_SIZE) {
		fprintf(stderr, "Error: read %d samples\n", r);
		exit(ERR_READ);
	}

	struct audio_fader af;
	init_af(&af);
	start_fade(&af, FADE_OUT, 100, 0);
	process(&af, (const short *)pcm_buf, (short *)pcm_buf, BUF_SIZE);

	FILE *output = fopen(FILE_NAME_OUT, "w");
	if (output == 0) {
		fprintf(stderr, "Failed to open file: %s\n", FILE_NAME_OUT);
		exit(ERR_OPEN);
	}

	int w = fwrite((const void *)pcm_buf, sizeof(int), BUF_SIZE, output);
	if (w != BUF_SIZE) {
		fprintf(stderr, "Error: write %d samples\n", w);
		exit(ERR_WRITE);
	}

	fclose(pcm);
	fclose(output);

	exit(0);
}
