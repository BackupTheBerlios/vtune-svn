/***************************************************************************
 *   Copyright (C) 2007 by Deryabin Andrew   *
 *   andrewderyabin@newmail.ru   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <jack/jack.h>
#include <samplerate.h>
#include <complex>
#include <math.h>
#include <assert.h>

#include <vtune-scale.h>
#include <vtune-fft.h>
#include <vtune-filter.h>
#include <vtune-track.h>

#define VTUNE_MAJOR 0
#define VTUNE_MINOR 0
#define VTUNE_RELEASE 1

#define VTUNE_CORE_VERSION (VTUNE_MAJOR << 16) | (VTUNE_MINOR << 8) | (VTUNE_RELEASE << 8)

#define VTUNE_DBG(x...) fprintf(stderr, "vTune:" x); fprintf(stderr, "\n")

struct vtune_data
{
	void *arg;
	double *fft_mag;
	fft_type *fft_res;
	unsigned short fft_size;
	float *samples;
	unsigned short samples_size;
	double peek;
	double index;
	double freq;
	vtune_scale scale;
	double shift;
	bool valid;
	bool solid;
};

typedef void VTUNE_CALLBACK(vtune_data *data);

extern bool vtune_get_note(const double &freq, vtune_scale &scale, double &shift);

class vTune
{
public:

	vTune();
	~vTune();
	bool Init();
	void SetCallback(VTUNE_CALLBACK *_callback, void *_arg);
	unsigned short GetSize()
	{
		return tracker->GetSize();
	};
	vTuneTrack *GetTracker();
private:
	static int Process(jack_nframes_t nframes, void *arg);
	void TrackData();
	unsigned short GetPeeks(const double *spectrum, unsigned short size);
	jack_client_t *jack_handle;
	jack_port_t *jack_input;
	jack_port_t *jack_output;
	jack_nframes_t sample_rate;
	jack_nframes_t buffer_size;
	unsigned short track_size;
	unsigned short track_tail;
	vtune_data data;
	bool track_enabled;
	jack_default_audio_sample_t *track_buffer;
	VTUNE_CALLBACK *callback;
	void *callback_arg;
	vTuneTrack *tracker;
	unsigned short process_size;
};
