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
	fftmag_length = 0;
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
	QPen pen1(QColor(0, 0, 0));
	QPen pen2(QColor(200, 200, 200));
	QPen pen3(QColor(0, 0, 255));
	for (unsigned short i = 0; i < width; i ++)
	{
		double delta = fftmag_lut [i] - fftmag [i];
		if (delta < 0)
			fftmag_lut [i] = fftmag [i];
		else
			fftmag_lut [i] -= (delta / 4);
		int pt = fftmag_lut [i] * height;
		int k = i;
		if (i == max_peek)
			painter->setPen(pen3);
		else
			painter->setPen(pen1);
		painter->drawLine(k, height, k, height - pt);
		if (pt < height)
		{
			painter->setPen(pen2);
			painter->drawLine(k, height - pt, k, 0);
		}

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

void vSpectrum::SetData(double *_fftmag, unsigned short _fftmag_length, unsigned short _max_peek)
{
	if (fftmag_length != _fftmag_length)
	{
		fftmag_length = _fftmag_length;
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

	if (!fftmag)
	{
		fftmag = new double [fftmag_length];
	}

	if (!fftmag_lut)
	{
		fftmag_lut = new double [fftmag_length];
		memset(fftmag_lut, 0, fftmag_length * sizeof(double));
	}

	memcpy(fftmag, _fftmag, fftmag_length * sizeof(double));
	max_peek = _max_peek;
}

