/**
*  @file Application.h
*
*  Application class.
*
*  Author : Vladimir O. Ovchinnikov
**/

#include <QApplication>
#include <QString>

class Application : public QApplication
{
	Q_OBJECT
public:
	Application(int& argc, char** argv);
	virtual ~Application() = default;

	int run();
};
