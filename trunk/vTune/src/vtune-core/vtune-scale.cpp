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

vtune_scale piano_scale [] =
    {
        {0,    "",		0.0000},
        {88,   "C8",           4186.01},
        {87,   "B7",           3951.07},
        {86,   "A#7/Bb7",      3729.31},
        {85,   "A7",           3520.00},
        {84,   "G#7/Ab7",      3322.44},
        {83,   "G7",           3135.96},
        {82,   "F#7/Gb7",      2959.96},
        {81,   "F7",           2793.83},
        {80,   "E7",           2637.02},
        {79,   "D#7/Eb7",      2489.02},
        {78,   "D7",           2349.32},
        {77,   "C#7/Db7",      2217.46},
        {76,   "C7",           2093.00},
        {75,   "B6",           1975.53},
        {74,   "A#6/Bb6",      1864.66},
        {73,   "A6",           1760.00},
        {72,   "G#6/Ab6",      1661.22},
        {71,   "G6",           1567.98},
        {70,   "F#6/Gb6",      1479.98},
        {69,   "F6",           1396.91},
        {68,   "E6",           1318.51},
        {67,   "D#6/Eb6",      1244.51},
        {66,   "D6",           1174.66},
        {65,   "C#6/Db6",      1108.73},
        {64,   "C6",           1046.50},
        {63,   "B5",            987.767},
        {62,   "A#5/Bb5",       932.328},
        {61,   "A5",            880.000},
        {60,   "G#5/Ab5",       830.609},
        {59,   "G5",            783.991},
        {58,   "F#5/Gb5",       739.989},
        {57,   "F5",            698.456},
        {56,   "E5",            659.255},
        {55,   "D#5/Eb5",       622.254},
        {54,   "D5",            587.330},
        {53,   "C#5/Db5",       554.365},
        {52,   "C5",            523.251},
        {51,   "B4",            493.883},
        {50,   "A#4/Bb4",       466.164},
        {49,   "A4 (A440)",     440.000},
        {48,   "G#4/Ab4",       415.305},
        {47,   "G4",            391.995},
        {46,   "F#4/Gb4",       369.994},
        {45,   "F4",            349.228},
        {44,   "E4",            329.628},
        {43,   "D#4/Eb4",       311.127},
        {42,   "D4",            293.665},
        {41,   "C#4/Db4",       277.183},
        {40,   "C4 (middle C)", 261.626},
        {39,   "B3",            246.942},
        {38,   "A#3/Bb3",       233.082},
        {37,   "A3",            220.000},
        {36,   "G#3/Ab3",       207.652},
        {35,   "G3",            195.998},
        {34,   "F#3/Gb3",       184.997},
        {33,   "F3",            174.614},
        {32,   "E3",            164.814},
        {31,   "D#3/Eb3",       155.563},
        {30,   "D3",            146.832},
        {29,   "C#3/Db3",       138.591},
        {28,   "C3",            130.813},
        {27,   "B2",            123.471},
        {26,   "A#2/Bb2",       116.541},
        {25,   "A2",            110.000},
        {24,   "G#2/Ab2",       103.826},
        {23,   "G2",             97.9989},
        {22,   "F#2/Gb2",        92.4986},
        {21,   "F2",             87.3071},
        {20,   "E2",             82.4069},
        {19,   "D#2/Eb2",        77.7817},
        {18,   "D2",             73.4162},
        {17,   "C#2/Db2",        69.2957},
        {16,   "C2",             65.4064},
        {15,   "B1",             61.7354},
        {14,   "A#1/Bb1",        58.2705},
        {13,   "A1",             55.0000},
        {12,   "G#1/Ab1",        51.9130},
        {11,   "G1",             48.9995},
        {10,   "F#1/Gb1",        46.2493},
        {9,    "F1",             43.6536},
        {8,    "E1",             41.2035},
        {7,    "D#1/Eb1",        38.8909},
        {6,    "D1",             36.7081},
        {5,    "C#1/Db1",        34.6479},
        {4,    "C1",             32.7032},
        {3,    "B0",             30.8677},
        {2,    "A#0/Bb0",        29.1353},
        {1,    "A0",             27.5000},
        {0,    "",		  0.0000}
    };

#define PIANO_SCALE_SIZE sizeof(piano_scale) / sizeof(piano_scale [0])

vTuneScale::vTuneScale()
{
	track_latency = track_current = 0;
	piano_scale_size = PIANO_SCALE_SIZE;
}

void vTuneScale::Init(jack_nframes_t sample_rate, jack_nframes_t buffer_size)
{
	track_latency = ((sample_rate / 1000) * 50) / buffer_size;
	track_current = 0;
}

bool vTuneScale::GetNote(const double &freq, vtune_scale &scale, double &shift)
{
 	unsigned short i = piano_scale_size - 2;
	bool bValid = false;

	shift = 0;
	while (piano_scale [i].num != 0)
	{
		double avg = (piano_scale [i + 1].freq + piano_scale [i].freq) / 2;
		if (freq > piano_scale [i + 1].freq && freq <= avg)
		{
			i++;
			break;
		}
		else if (freq > avg && freq <= piano_scale [i].freq)
		{
			break;
		}
		i--;
	}


	if (piano_scale [i].freq == 0)
		return bValid;

	/*if(scale.num == piano_scale [i].num)
	{
		if(track_current < track_latency)
		{
			track_current++;
		}
		else
		{
			bValid = true;
		}
	}
	else
		track_current = 0;*/

	scale.num = piano_scale [i].num;
	scale.note = piano_scale [i].note;
	scale.freq = piano_scale [i].freq;
	shift = piano_scale [i].freq - freq;
	return true;
}
