/**
*  @file PopupMessage.h
*
*  Popup message
*
*  Author : Vladimir O. Ovchinnikov
**/

#pragma once

#include <QObject>
#include <QLabel>

class PopupMessage : public QObject
{
	Q_OBJECT
public:
	explicit PopupMessage(QWidget* parent);

public slots:
	void show(QString message, int milliseconds = 3000);
	void cancel();

protected:
	void timerEvent(QTimerEvent* event) override;

private:
	QLabel* messageLabel { nullptr };
	int timerId;
	int64_t messageEndTime { 0 };

	void timerProc();
};
