/**
*  @file MainWindow.cpp
*
*  Main program window implementation file
*
*  Author : Vladimir O. Ovchinnikov
**/

#include "Mainwindow.h"
#include "Application.h"

#include "QSettings"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, Ui::MainWindow()
{
	setupUi(this);

	popupMessage = new PopupMessage(this);

	// Генерация поля
	connect(pb_generate, &QPushButton::clicked, this, &MainWindow::generate);

	graph = new SearchGraph(sb_width->value(), sb_height->value());

	// Вывод сообщения
	connect(graph, &SearchGraph::showMessage, this, [=](QString text, int millisec){
		popupMessage->show(text, millisec);
	});

	gridLayout->addWidget(graph);
}

void MainWindow::generate()
{
	graph->paintField(sb_width->value(), sb_height->value(), true);
}

/**
 * @brief Сохранение конфигурации положения и размера окна
 */
void MainWindow::saveConfig()
{
	QSettings qsettings(QSettings::IniFormat, QSettings::SystemScope, "--", tr(APP_NAME));

	qsettings.beginGroup("MainWindow");

	qsettings.setValue("geometry", saveGeometry());
	qsettings.setValue("maximized", isMaximized());
	if (!isMaximized())
	{
		qsettings.setValue("pos", pos());
		qsettings.setValue("size", size());
	}
	qsettings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	saveConfig();
	QMainWindow::closeEvent(event);
}


