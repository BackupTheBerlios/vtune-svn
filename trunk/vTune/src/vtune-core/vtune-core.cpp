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

#include <vtune-core.h>

vTune::vTune()
{

	track_buffer = 0;

}

vTune::~vTune()
{
	if (jack_handle)
	{
		jack_deactivate(jack_handle);
		jack_client_close(jack_handle);
	}

	if(track_buffer)
		delete [] track_buffer;
	if(tracker)
		delete tracker;

}

bool vTune::Init()
{
	VTUNE_DBG("core version: %.3d.", VTUNE_CORE_VERSION);
	int nl = jack_client_name_size();
	char *client_name = new char [nl];
	pid_t pid = getpid();
	snprintf(client_name, nl, "%s - %d", "vTune", pid);
	jack_handle = jack_client_new(client_name);
	if (jack_handle == NULL)
	{
		VTUNE_DBG("Can't connect to jack server!\n");
		return false;
	}

	callback = 0;

	sample_rate = jack_get_sample_rate(jack_handle);
	buffer_size = jack_get_buffer_size(jack_handle);
	VTUNE_DBG("Sample rate is %d Hz, buffer size is %d", sample_rate, buffer_size);

	// This is a tracking window for pitch detection
	// Higher values will provide more accurate results
	// but will add latency
	//

	process_size = 1024 * 2;

	tracker = new vTuneTrack(VTRACK_HPS, process_size, sample_rate, buffer_size);

	track_buffer = new float [process_size];

	data.valid = false;

	track_enabled = false;
	track_tail = 0;
	if (process_size <= buffer_size)
	{
		track_size = buffer_size / process_size;
		VTUNE_DBG("Tracking disabled: %d tails.", track_size);
	}
	else
	{
		track_size = process_size / buffer_size;
		track_enabled = true;
		VTUNE_DBG("Tracking enabled: %d tails.", track_size);
	}

	//Ok. Create input and output ports.
	jack_input = jack_port_register(jack_handle, "input0", JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
	if (!jack_input)
	{
		VTUNE_DBG("Can't create input port!\n");
		jack_client_close(jack_handle);
		jack_handle = NULL;
		return false;
	}

	jack_output = jack_port_register(jack_handle, "output0", JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
	if (!jack_output)
	{
		VTUNE_DBG("Can't create output port!\n");
		jack_port_unregister(jack_handle, jack_input);
		jack_client_close(jack_handle);
		jack_handle = NULL;
		jack_input = NULL;
		return false;
	}

	jack_set_process_callback(jack_handle, vTune::Process, this);
	//Activate client
	if (jack_activate(jack_handle))
	{
		VTUNE_DBG("Can't activate client!\n");
		jack_client_close(jack_handle);
		jack_handle = NULL;
		return false;
	}

	return true;
}

int vTune::Process(jack_nframes_t nframes, void *arg)
{
	vTune *vtune = (vTune *) arg;
	jack_default_audio_sample_t *in = (jack_default_audio_sample_t *) jack_port_get_buffer(vtune->jack_input, nframes);
	jack_default_audio_sample_t *out = (jack_default_audio_sample_t *) jack_port_get_buffer(vtune->jack_output, nframes);

	if (vtune->track_enabled)
	{
		if (vtune->track_tail < vtune->track_size)
		{
			memcpy(vtune->track_buffer + vtune->track_tail * nframes, in, nframes * sizeof(jack_default_audio_sample_t));
			/*unsigned short offset = vtune->track_tail * nframes;
			for (unsigned short i = 0; i < nframes; i++)
			{
				vtune->track_buffer [i + offset] = in [i];
			}*/
			vtune->track_tail++;
			return 0;
		}
		else
		{
			memcpy(vtune->track_buffer, vtune->track_buffer + nframes, nframes * (vtune->track_size - 1) * sizeof(jack_default_audio_sample_t));
			memcpy(vtune->track_buffer + nframes * (vtune->track_size - 1), in, nframes * sizeof(jack_default_audio_sample_t));
			/*unsigned short size = nframes * (vtune->track_size - 1);
			for (unsigned short i = 0; i < size; i++)
			{
				vtune->track_buffer [i] = vtune->track_buffer [i + nframes];
			}
			for (unsigned short i = 0; i < nframes; i++)
			{
				vtune->track_buffer [i + size] = in [i];
			}*/
		}

		vtune->TrackData();
	}
	else
	{
		for (unsigned short i = 0; i < vtune->track_size; i++)
		{
			memcpy(vtune->track_buffer, in, (nframes / vtune->track_size) * sizeof(jack_default_audio_sample_t));
			vtune->TrackData();
		}
	}

	//vtune->bp_filter->Process(in, out, false);
	
	memcpy(out, in, nframes * sizeof(jack_default_audio_sample_t));
	return 0;
}
void vTune::SetCallback(VTUNE_CALLBACK *_callback, void *_arg)
{
	callback_arg = _arg;
	callback = _callback;
 	data.arg = callback_arg;
}

void vTune::TrackData()
{
	tracker->Track(track_buffer, &data);
	/*static double *buff = 0;
	if(!buff)
		buff = new double [process_size];

	//double max_peek = 0.1;
	
	for(unsigned short i = 0; i < process_size; i++)
	{
		buff [i] = fabs(track_buffer [i]);
		if(buff [i] > max_peek)
			max_peek = buff [i];
	}

	VTUNE_DBG("max_peek = %f", max_peek);
	
	for(unsigned short i = 0; i < process_size; i++)
	{
		buff [i] /= max_peek;
	}
	
	data.fft_mag = buff;*/
	if(callback)
		callback(&data);
}
