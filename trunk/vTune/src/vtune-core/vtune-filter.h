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
class vTuneFilter;

enum vFilterType
{
	LPF = 0,
 	HPF,
 	BPF
};

struct vFilterOpts
{
	jack_nframes_t Fs; //sample rate
	double f0; //center freq
	double bw; //bandwidth
	vFilterType type; //filter type
};

class vTuneFilter
{
public:	
	vTuneFilter();
	~vTuneFilter();
	void Init(const vFilterOpts *opts, jack_nframes_t _buffer_size);
	void Process(jack_default_audio_sample_t *in, jack_default_audio_sample_t *out, bool in_place = true);
	void Cascade(vTuneFilter *_next);
private:
	jack_nframes_t buffer_size;
	double a0,a1,a2;
	double b0,b1,b2;
	double b0_a0, b1_a0, b2_a0;
	double a1_a0, a2_a0;
	jack_default_audio_sample_t *buffer;
	double in_delay_line [2];
	double out_delay_line [2];
	vTuneFilter *next;
};
