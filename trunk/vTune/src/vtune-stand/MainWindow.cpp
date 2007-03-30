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

vMainWindow::vMainWindow(QWidget *Parent) : QWidget(Parent)
{
	spectrum = new vSpectrum(this);

	spectrum->show();

	QPushButton *quitbtn = new QPushButton("QUIT", this);
	quitbtn->resize(QSize(100, 50));
	quitbtn->move(0, 301);
	quitbtn->show();
	freqText = new QLabel(this);
	freqMag = new QLabel(this);
	freqText->move(105, 301);
	freqMag->move(105, 335);
	freqText->resize(QSize(200, 20));
	freqMag->resize(QSize(200, 20));
	freqText->show();
	freqMag->show();
	connect(quitbtn, SIGNAL(clicked()), qApp, SLOT(quit()));
	connect(this, SIGNAL(spectrumChanged(vtune_data *)), this, SLOT(RepaintSpectrum(vtune_data *)));
	vTune *vtune = new vTune();
	if (!vtune->Init())
	{
		exit(1);
	}
	int width = vtune->GetSize() / 2;
	width = width > 512 ? 512 : width;
	spectrum->resize(QSize(width, 300));
	splitter = new QSplitter(Qt::Vertical, this);
	splitter->addWidget(spectrum);
	splitter->addWidget(quitbtn);
	splitter->addWidget(freqText);
	splitter->addWidget(freqMag);
	splitter->resize(QSize(width, 500));
	splitter->show();
	vtune->SetCallback(callback, this);
}

void vMainWindow::Close()
{
}

void vMainWindow::RepaintSpectrum(vtune_data *data)
{
	QString noteData = QString("Not valid!");
	if(data->valid)
		noteData = QString(data->scale.note) + QString(", freq:") + QString::number(data->scale.freq) + QString("-") + QString::number(data->shift) + QString("(shift)");
	
		
	freqText->setText(QString("Peek: ") + QString::number(data->index) + QString(",freq: ") + QString::number(data->freq) + QString("(") + QString::number(data->peek) + QString(")"));
	freqMag->setText(QString("Note: ") + noteData);
	spectrum->update();
}

void vMainWindow::callback(vtune_data *data)
{
	vMainWindow *mainWindow = (vMainWindow *)data->arg;
	mainWindow->spectrum->SetData(data);
	mainWindow->emit spectrumChanged(data);
}
