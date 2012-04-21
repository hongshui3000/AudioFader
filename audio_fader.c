/*
 * audio_fader.c
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
#include <string.h>
#include "audio_fader.h"

#define SAMPLE_RATE    44100
#define SHIFT          14

static const int max_gain = 1 << SHIFT;
static const float sample_per_ms = SAMPLE_RATE / 1000.0f;
static const int max_duration = (1 << SHIFT) / SAMPLE_RATE * 1000;

// Local utility functions
static void reset(struct audio_fader *af)
{
	af->duration = 0;
	af->init = max_gain;
	af->target = max_gain;
	af->step = 0;
	af->enable_fade = false;
}

static inline int multi_rl(int left, unsigned int in_rl, unsigned int v_rl)
{
	if (left) {
		return (short)(in_rl & 0xFFFF) * (short)(v_rl & 0xFFFF);
	} else {
		return (short)(in_rl >> 16) * (short)(v_rl >> 16);
	}
}

// Public interfaces
int init_af(struct audio_fader *af)
{
	reset(af);
}

void exit_af(struct audio_fader *af)
{
}

int start_fade(struct audio_fader *af, enum fade_type type, unsigned int duration_msec, unsigned int target)
{
	if (duration_msec <= 0 || duration_msec > max_duration) {
		af->duration = max_duration;
	}
	af->duration = duration_msec;

	if (target < 0 || target > MAX_TARGET)
		return INVALID;
	af->target = max_gain * target / MAX_TARGET;

	if (type == FADE_IN) {
		af->init = 0;
		af->step = (target / (duration_msec * sample_per_ms));
	} else if (type == FADE_OUT) {
		af->init = max_gain;
		af->step = ((target - max_gain) / (duration_msec * sample_per_ms));
	} else {
		return INVALID;
	}

	af->enable_fade = true;

	return OK;
}

void stop_fade(struct audio_fader *af)
{
	af->enable_fade = false;
}

// process frame_count samples in in_buffer and copy them into out_buffer
void process(struct audio_fader *af, const short *in_buffer, short *out_buffer, unsigned int frame_count)
{
	if (!af->enable_fade) {
		if (in_buffer == out_buffer) {
			return;
		} else {
			memcpy((void *)out_buffer, (const void *)in_buffer, frame_count * 4); // here assumes stereo 16-bit pcm
			return;
		}
	}
	//TODO: process data when enabled
	const unsigned int *in_rl = (unsigned int const *)in_buffer;
	unsigned int *out = (unsigned int *)out_buffer;
	unsigned int vrl = (af->target << 16) | (af->target & 0xFFFF);

	do {
		if (af->step != 0) {
			if ((af->step > 0 && af->init + af->step < af->target)
				|| (af->step < 0 && af->init + af->step > af->target)) {
				af->init += af->step;
				vrl = (af->init << 16) | (af->init & 0xFFFF);
			} else {
				af->step = 0;
				vrl = (af->target << 16) | (af->target & 0xFFFF);
			}
		}

		int l = multi_rl(1, *in_rl, vrl) >> SHIFT;
		int r = multi_rl(0, *in_rl, vrl) >> SHIFT;
		*out++ = (r << 16) | (l & 0xFFFF);
		in_rl++;
	} while (--frame_count);
}

