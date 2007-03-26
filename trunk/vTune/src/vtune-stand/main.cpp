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
#include <signal.h>

void sig_handler(int sig)
{
	switch (sig)
	{
	case SIGINT:
		VTUNE_DBG("Exit on signal.");
		exit(1);

		break;
	default:
		break;
	}
}

int main(int argc, char **argv)
{
	signal(SIGINT, sig_handler);
	printf("Initializing core..\n");
	QApplication app(argc, argv);
	vMainWindow *mainwnd = new vMainWindow();
	mainwnd->show();

	return app.exec();
}
