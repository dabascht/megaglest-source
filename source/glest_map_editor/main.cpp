// ==============================================================
//	This file is part of Glest (www.glest.org)
//
//	Copyright (C) 2001-2008 Martiño Figueroa
//
//	You can redistribute this code and/or modify it under
//	the terms of the GNU General Public License as published
//	by the Free Software Foundation; either version 2 of the
//	License, or (at your option) any later version
// ==============================================================

#include "main.h"
#include <ctime>
#include "conversion.h"
#include "platform_common.h"
#include "config.h"
#include <iostream>
#include "platform_util.h"
#ifndef WIN32
#include <errno.h>
#endif
#include <memory>

using namespace Shared::Util;
using namespace Shared::PlatformCommon;
using namespace Glest::Game;
using namespace std;

namespace Glest { namespace Game {
string getGameReadWritePath(string lookupKey) {
	string path = "";
	if(path == "" && getenv("GLESTHOME") != NULL) {
		path = getenv("GLESTHOME");
		if(path != "" && EndsWith(path, "/") == false && EndsWith(path, "\\") == false) {
			path += "/";
		}

		//SystemFlags::OutputDebug(SystemFlags::debugSystem,"In [%s::%s Line: %d] path to be used for read/write files [%s]\n",__FILE__,__FUNCTION__,__LINE__,path.c_str());
	}

	return path;
}
}}








#include "main.h"
#include "ui_main.h"
//namespace MapEditor {
	MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MainWindow)
	{
		ui->setupUi(this);
		newmap = new NewMap();//instance of new map dialog
		connect(ui->actionNew, SIGNAL(triggered()), newmap, SLOT(show()));//open dialog window
	}

	MainWindow::~MainWindow()
	{
		delete ui;
		delete newmap;
	}

	//for translation ... somehow
	void MainWindow::changeEvent(QEvent *e)
	{
		QMainWindow::changeEvent(e);
		switch (e->type()) {
		case QEvent::LanguageChange:
			ui->retranslateUi(this);
			break;
		default:
			break;
		}
	}
//}// end namespace


//initialize and open the window
int main(int argc, char *argv[]){
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}
