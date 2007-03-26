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

typedef std::complex<double> fft_type;
#define PI 3.14159265
class vTuneFFT
{
public:
	vTuneFFT(unsigned short _Points, double _SampleRate);
	~vTuneFFT();
	void SetSamples(float *buffer, unsigned short _Points = 0);
	void SetSamples(double *buffer, unsigned short _Points = 0);
	void SetSamples(fft_type *buffer, unsigned short _Points = 0);
	void GetSamples(float *buffer, unsigned short _Points = 0);
	void GetSamples(double *buffer, unsigned short _Points = 0);
	void GetSamples(fft_type *buffer, unsigned short _Points = 0);
	fft_type *GetSamples();
	void DoFFT();
	void DoInverseFFT();
	double GetFreq(unsigned short i)
	{
		return SRLut [i];
	};
	double Modulus(unsigned short i)
	{
		return sqrt(Data [i].real() *Data [i].real() + Data [i].imag() * Data [i].imag());
	};
private:
	unsigned short Points;
	unsigned short Stages;
	fft_type *Data;
	fft_type *ExpLut;
	fft_type *Buffer;
	double *SRLut;
	unsigned short *BRLut;
	double SampleRate;
	double divider;
	void Sort();
};

