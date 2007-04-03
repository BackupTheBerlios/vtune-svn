//
// C++ Interface: vtunetrack
//
// Description: 
//
//
// Author: Deryabin Andrew <andrewderyabin@newmail.ru>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VTUNETRACK_H
#define VTUNETRACK_H

struct vtune_data;

enum vtune_track_type
{
	VTRACK_FFT1 = 0,
 	VTRACK_ACF,
  	VTRACK_HPS
};

class vTuneTrack
{
public:
	vTuneTrack(vtune_track_type _type, unsigned short _size, jack_nframes_t _sample_rate, jack_nframes_t _buffer_size);
	~vTuneTrack();
	void Track(jack_default_audio_sample_t *buffer, vtune_data *data);
	unsigned short GetSize()
	{
		return process_size;
	};
	void SetType(vtune_track_type _type);
private:
	void TrackFFT1(jack_default_audio_sample_t *buffer, vtune_data *data);
	void TrackACF(jack_default_audio_sample_t *buffer, vtune_data *data);
	void TrackHPS(jack_default_audio_sample_t *buffer, vtune_data *data);
	unsigned short GetPeeks(const double *spectrum, unsigned short size, unsigned short range_left, unsigned short range_right, double treshhold);
	short GetPeriod(const double *samples, unsigned short size, unsigned short start, unsigned short stop, double treshhold);
	vtune_track_type type;
	vTuneFFT *fft;
	vTuneFFT *fft1;
	vTuneFilter *lp_filter;
	vTuneFilter *hp_filter;
	vTuneFilter *bp_filter;
	vTuneScale *scale;
	jack_default_audio_sample_t *signal;
	jack_default_audio_sample_t *filtered;
	unsigned short *peeks;
	double *fft_mag;
	double *hps;
	double *window;
	unsigned short process_size;
	jack_nframes_t sample_rate;
	jack_nframes_t buffer_size;
	unsigned short peek_range_left;
	unsigned short peek_range_right;
	unsigned short fft_half_size;
	short last_period;
};

#endif
