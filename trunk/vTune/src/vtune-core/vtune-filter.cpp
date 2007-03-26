/***************************************************************************
 *   Copyright (C) 2007 by Deryabin Andrew   				   *
 *   andrewderyabin@newmail.ru   					   *
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

#include <vtune-core.h>

vTuneFilter::vTuneFilter()
{
	buffer = 0;
}

vTuneFilter::~vTuneFilter()
{
	if(buffer)
		delete [] buffer;
}

void vTuneFilter::Init(const vFilterOpts *opts, jack_nframes_t _buffer_size)
{
	unsigned long Fs = opts->Fs;
	double f0 = opts->f0;
	double bw = opts->bw;
	buffer_size = _buffer_size;

	assert(Fs != 0);
	
	assert(buffer_size > 2);

	if(buffer)
		delete [] buffer;

	buffer = new jack_default_audio_sample_t [buffer_size];
	memset(buffer, 0, buffer_size * sizeof(jack_default_audio_sample_t));

	assert(buffer != 0);

	double w0 = 2 * PI * f0 / Fs;
	
	assert(w0 != 0);

	in_delay_line [0] = in_delay_line [1] = 0;
	out_delay_line [0] = out_delay_line [1] = 0;

	double alpha = sin(w0) * sinh((log(2) / 2) * bw * (w0 / sin(w0)));
	double one_q = 2 * sinh(log(2) / 2 * bw * w0 / sin(w0));
	
	switch(opts->type)
	{
	case LPF:
		b0 = (1 - cos(w0)) / 2;
		b1 = 1 - cos(w0);
		b2 = (1 - cos(w0)) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cos(w0);
		a2 = 1 - alpha;
		break;
	case HPF:
		b0 = (1 + cos(w0)) / 2;
		b1 = -(1 + cos(w0));
		b2 = (1 + cos(w0)) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cos(w0);
		a2 = 1 - alpha;
		break;
	case BPF:
		b0 = sin(w0) / 2;
		b1 = 0;
		b2 = -sin(w0) / 2;
		a0 = 1 + alpha;
		a1 = -2 * cos(w0);
		a2 = 1 - alpha;
		break;
	default:
		b0 = b1 = b2 = a0 = a1 = a2 = 0;
		b0_a0 = b1_a0 = b2_a0 = a1_a0 = a2_a0 = 0;
		return;
	}

	b0_a0 = b0 / a0;
	b1_a0 = b1 / a0;
	b2_a0 = b2 / a0;
	a1_a0 = a1 / a0;
	a2_a0 = a2 / a0;
}

void vTuneFilter::Process(jack_default_audio_sample_t *in, jack_default_audio_sample_t *out, bool in_place)
{
	//static int counter = 0;
	float *process_buffer = in_place ? buffer : out;
	//VTUNE_DBG("%d", counter);
	//counter++;
	
	process_buffer [0] = b0_a0 * in [0] + b1_a0 * in_delay_line [1] + b2_a0 * in_delay_line [0]
					    - a1_a0 * out_delay_line [1] - a2_a0 * out_delay_line [0];
	process_buffer [1] = b0_a0 * in [1] + b1_a0 * in [0] + b2_a0 * in_delay_line [1]
			- a1_a0 * out [0] - a2_a0 * out_delay_line [1];
	
	for(unsigned short i = 2; i < buffer_size; i++)
	{
		process_buffer [i] = b0_a0 * in [i] + b1_a0 * in [i - 1] + b2_a0 * in [i - 2]
						    - a1_a0 * process_buffer [i - 1]
						    - a2_a0 * process_buffer [i - 2];
	}

	if(in_place)
		memcpy(out, process_buffer, buffer_size * sizeof(jack_default_audio_sample_t));

	in_delay_line [0] = in [buffer_size - 2];
	in_delay_line [1] = in [buffer_size - 1];

	out_delay_line [0] = process_buffer [buffer_size - 2];
	out_delay_line [1] = process_buffer [buffer_size - 1];
}
