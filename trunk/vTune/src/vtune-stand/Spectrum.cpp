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

#include <vtune.h>

vSpectrum::vSpectrum(QWidget *Parent) : QWidget(Parent)
{
	fftmag = 0;
	fftmag_lut = 0;
	fft_size = 0;
	samples = 0;
}

void vSpectrum::paint(QPainter *painter, QPaintEvent *event)
{
	if (!fftmag || !fftmag_lut)
		return;
	static int k = 0;
	//painter->fillRect(event->rect(), QBrush(QColor(40,40,40)));
	painter->setPen(QPen(QColor(200, 200, 200)));
	int width = event->rect().width();
	//width = width < 512 ? width : 512;
	int height = event->rect().height();
	int half_height = height >> 1;
	
	QPen pen1(QColor(0, 0, 0));
	QPen pen2(QColor(200, 200, 200));
	QPen pen3(QColor(0, 0, 255));
	painter->setPen(pen1);
	painter->drawLine(0, half_height, width, half_height);
	int last_pt = 0;
	for (unsigned short i = 0; i < width; i ++)
	{
		double delta = fftmag_lut [i] - fftmag [i];
		if (delta < 0)
			fftmag_lut [i] = fftmag [i];
		else
			fftmag_lut [i] -= (delta / 4);
		int pt = fftmag_lut [i] * half_height;
		if (i == max_peek)
			painter->setPen(pen3);
		else
			painter->setPen(pen1);
		painter->drawLine(i, height, i, height - pt);
		/*if (pt < height)
		{
			painter->setPen(pen2);
			painter->drawLine(i, height - pt, i, half_height);
		}*/
		
		int offset = (half_height >> 1);

		painter->setPen(pen1);
		pt = samples [i] * half_height;
		pt = offset + pt;

		if(!i)
			painter->drawLine(i, pt, i, pt);
		else
			painter->drawLine(i - 1, last_pt, i, pt);
		//painter->setPen(pen2);
		//painter->drawLine(i, half_height - pt, i, half_height - pt - 1);
		
		//painter->drawLine(i, half_height, i, half_height - pt);
		//painter->drawPoint(i, pt);
		last_pt = pt;
	}

}


void vSpectrum::paintEvent(QPaintEvent *event)
{
	QPainter painter;
	painter.begin(this);
	//painter.setRenderHint(QPainter::Antialiasing);
	paint(&painter, event);
	painter.end();
}

void vSpectrum::SetData(vtune_data *data)
{
	if (data->fft_size != fft_size)
	{
		fft_size = data->fft_size;
		if (fftmag)
		{
			delete [] fftmag;
			fftmag = 0;
		}

		if (fftmag_lut)
		{
			delete [] fftmag_lut;
			fftmag_lut = 0;
		}
	}

	if(data->samples_size != samples_size)
	{
		samples_size = data->samples_size;
		if(samples)
		{
			delete [] samples;
			samples = 0;
		}
		
	}

	if (!fftmag)
	{
		fftmag = new double [fft_size];
	}

	if (!fftmag_lut)
	{
		fftmag_lut = new double [fft_size];
		memset(fftmag_lut, 0, fft_size * sizeof(double));
	}

	if(!samples)
	{
		samples = new float [samples_size];
	}

	memcpy(fftmag, data->fft_mag, fft_size * sizeof(double));
	memcpy(samples, data->samples, samples_size * sizeof(float));
	max_peek = data->index;
}

