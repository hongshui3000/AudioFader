/*
 * audio_fader.h
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

#define MAX_TARGET     100
#define MAX_DURATION   350

#ifdef __cplusplus 
extern "C" {
#endif

#define true 1
#define false 0
#define OK 0
#define INVALID -1

enum fade_type {
	FADE_IN,
	FADE_OUT,
};

struct audio_fader {
	int duration;
	int init;
	int target;
	int step;
	_Bool enable_fade;
};

// Public interfaces
int init_af(struct audio_fader *af);

void exit_af(struct audio_fader *af);

int start_fade(struct audio_fader *af, enum fade_type type, int duration_msec, int target);

void stop_fade(struct audio_fader *af);

// process frame_count samples in in_buffer and copy them into out_buffer
void process(struct audio_fader *af, const short *in_buffer, short *out_buffer, unsigned int frame_count);

#ifdef __cplusplus 
}
#endif

