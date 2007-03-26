//
// C++ Implementation: vtunetrack
//
// Description: 
//
//
// Author: Deryabin Andrew <andrewderyabin@newmail.ru>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <vtune-core.h>

vTuneTrack::vTuneTrack(vtune_track_type _type, unsigned short _size, jack_nframes_t _sample_rate, jack_nframes_t _buffer_size)
{
	type = _type;
	process_size = _size;
	sample_rate = _sample_rate;
	buffer_size = _buffer_size;

	fft = new vTuneFFT(process_size, sample_rate);
	fft1 = new vTuneFFT(process_size, sample_rate);
	signal = new float [process_size];
	
	filtered = new float [process_size];
	fft_mag = new double [process_size];
	peeks = new unsigned short [process_size];

	scale = new vTuneScale();
	scale->Init(sample_rate, buffer_size);

	lp_filter = new vTuneFilter();
	hp_filter = new vTuneFilter();
	bp_filter = new vTuneFilter();
	
	vFilterOpts filter_opts;
	filter_opts.Fs = sample_rate;
	filter_opts.f0 = 2500;
	filter_opts.bw = 2;
	filter_opts.type = LPF;
	
	lp_filter->Init(&filter_opts, process_size);

	filter_opts.f0 = 100;
	filter_opts.type = HPF;

	hp_filter->Init(&filter_opts, process_size);

	filter_opts.f0 = 2500;
	filter_opts.type = BPF;

	bp_filter->Init(&filter_opts, process_size);

	//processing range. (100Hz - 3000Hz)
	peek_range_right = (sample_rate / 2) / 100;
	peek_range_left = (sample_rate / 2) / 3000;

	VTUNE_DBG("Processing range: %d - %d.", peek_range_left, peek_range_right);
}


vTuneTrack::~vTuneTrack()
{
	if(fft)
		delete fft;
	if(fft1)
		delete fft1;
	if(lp_filter)
		delete lp_filter;
	if(hp_filter)
		delete hp_filter;
	if(scale)
		delete scale;
	if(signal)
		delete [] signal;
	if(filtered)
		delete [] filtered;
	if(fft_mag)
		delete [] fft_mag;
	if(peeks)
		delete [] peeks;
}

void vTuneTrack::TrackFFT(jack_default_audio_sample_t *buffer, vtune_data *data)
{
	unsigned short fft_half_size = process_size >> 1;

	//filter signal and remove frequences above 4KHz and below 100Hz
	//lp_filter->Process(track_buffer, filtered, false);
	//hp_filter->Process(filtered, filtered, true);
	bp_filter->Process(buffer, filtered, false);
	//memcpy(filtered, track_buffer, process_size * sizeof(jack_default_audio_sample_t));
	
	// Take 1st derivative of signal
	// and multiply it by Hann window.
	
	signal [0] = 0;
	signal [process_size - 1] = 0;
	for (unsigned short i = 1; i < process_size - 1; i++)
	{
		signal [i] = sample_rate * (filtered [i] - filtered [i - 1]);
		//signal [i] = track_buffer [i];
		signal [i] *= 0.5 * (1 - cos(2 * PI * (double) i / ((double) process_size - 1)));
	}

	fft->SetSamples(signal, process_size);
	fft->DoFFT();
	fft_type *fft_res = fft->GetSamples();
		data->fft_res = fft_res;
	data->fft_size = process_size;
	data->fft_mag = fft_mag;
	double max_amp = 1;
	for (unsigned short i = 0; i < process_size; i++)
	{
		double re = fft_res [i].real();
		double im = fft_res [i].imag();
		double square = re * re + im * im;
		if (square == 0)
			data->fft_mag [i] = 0;
		else
			data->fft_mag [i] = sqrt(square);
	}
	
	
	
	fft1->SetSamples(data->fft_mag, process_size);
	fft1->DoFFT();
	fft_res = fft1->GetSamples();
	
	unsigned short start = 0;
	
	double treshhold = 0.1;
	for (unsigned short i = start; i < (fft_half_size); i++)
	{
		double omega = (2 * PI * (double) i) / (double) process_size;
		double divider = sqrt(2 * ((double) 1 - cos(omega)));
		double scale;
		if (divider == 0)
			scale = 1.0;
		else
			scale = omega / divider;
		double re = fft_res [i].real();
		double im = fft_res [i].imag();
		double square = re * re + im * im;
		if (square == 0)
			data->fft_mag [i] = 0;
		else
			data->fft_mag [i] = sqrt(square) * scale;
		if (max_amp < data->fft_mag [i])
			max_amp = data->fft_mag [i];
	}
	
	
	for (unsigned short i = 0; i < (fft_half_size); i++)
	{
		data->fft_mag [i] /= max_amp;
	}
	unsigned short num_peeks = GetPeeks(data->fft_mag, fft_half_size);
	
	if (num_peeks < 2 || (num_peeks >= fft_half_size >> 1))
	{
		data->peek = 0;
		data->index = 0;
		data->freq = 0;
	}
	else
	{
		unsigned short max_index = 0;
		double max_peek = 0;
		for (unsigned short i = 1; i < num_peeks; i++)
		{
			unsigned short index = peeks [i];
			double amp = data->fft_mag [index];
			if (max_peek < amp)
			{
				max_peek = amp;
				max_index = index;
			}
	
		}
	
		data->peek = max_peek;
		data->index = max_index;
		data->freq = (sample_rate / 2) / max_index;
	}
	
	data->valid = scale->GetNote(data->freq, data->scale, data->shift);
}

unsigned short vTuneTrack::GetPeeks(const double *spectrum, unsigned short size)
{
	unsigned short i = peek_range_left + 1;
	unsigned short index = 0;
	double treshhold = 0.1;
	unsigned short limit = size > (peek_range_right - 1)? (peek_range_right - 1) : size;
	unsigned short max_index = 0;
	double max_peek = 0;
	while (i < limit)
	{
		while ((i < limit) && (spectrum [i - 1] <= spectrum [i]))
		{
			i++;
		}
		if (spectrum [i - 1] > max_peek)
		{
			max_index = i - 1;
			max_peek = spectrum [i - 1];
		}

		while ((i < limit) && (spectrum [i - 1] >= spectrum [i]))
		{
			i++;
		}
		if ((spectrum [i - 1] < treshhold) && (max_peek > treshhold))
		{
			peeks [index++] = max_index;
			max_peek = 0;
		}

	}
	return index;

}

void vTuneTrack::TrackACF(jack_default_audio_sample_t *buffer, vtune_data *data)
{
}

void vTuneTrack::Track(jack_default_audio_sample_t *buffer, vtune_data *data)
{
	switch(type)
	{
	case VTRACK_FFT:
		TrackFFT(buffer, data);
		break;
	case VTRACK_ACF:
		TrackACF(buffer, data);
		break;
	default:
		break;
	}
}


