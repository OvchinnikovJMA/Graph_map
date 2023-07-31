/**
*  @file Application.cpp
*
*  Application class implementation.
*
*  Author : Vladimir O. Ovchinnikov
**/

#include <QSettings>

#include "Application.h"
#include "Mainwindow.h"

Application::Application(int &argc, char **argv)
	: QApplication(argc, argv)
{
	setApplicationName(APP_NAME);
	setApplicationDisplayName(APP_NAME);
	setApplicationVersion(APP_VERSION);
}

int Application::run()
{
	MainWindow w;
	w.setWindowTitle(tr(APP_NAME));

	// Load config
	QSettings qsettings(QSettings::IniFormat, QSettings::SystemScope, "--", tr(APP_NAME));

	qsettings.beginGroup("MainWindow");

	w.restoreGeometry(qsettings.value("geometry", w.saveGeometry()).toByteArray());
	if (qsettings.value("maximized", w.isMaximized()).toBool())
	{
		w.showMaximized();
	}
	else
	{
		w.move(qsettings.value("pos", w.pos()).toPoint());
		w.resize(qsettings.value("size", w.size()).toSize());
	}

	qsettings.endGroup();

	w.show();

	return exec();
}
