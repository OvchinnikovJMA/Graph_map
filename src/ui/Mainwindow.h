/**
*  @file MainWindow.h
*
*  Main program window
*
*  Author : Vladimir O. Ovchinnikov
**/

#pragma once

#include "ui_Mainwindow.h"
#include <QMainWindow>
#include "SearchGraph.h"
#include "PopupMessage.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
private:
	PopupMessage* popupMessage;

	uint16_t s_width;
	uint16_t s_height;

	SearchGraph *graph;

	void generate();

	void saveConfig();

	void closeEvent(QCloseEvent *event);
};
