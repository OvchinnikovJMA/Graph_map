/**
*  @file PopupMessage.h
*
*  Popup message
*
*  Author : Vladimir O. Ovchinnikov
**/

#include "PopupMessage.h"
#include <QTimerEvent>
#include <QDateTime>
#include <QStyle>

PopupMessage::PopupMessage(QWidget* parent)
	: QObject(parent)
{
	static constexpr char messageLabelStyle[] =
		"QLabel {"
		"  font-weight: bold;"
		"  font-size: 20px;"
		"  padding: 10px 15px;"
		"  border: 2px solid;"
		"  border-radius: 10px;"
		"  background-color: rgba(255, 153, 153, 200);"
		"  border-color: red;"
		"}"
		;

	messageLabel = new QLabel("", parent);
	messageLabel->setAlignment(Qt::AlignCenter);
	messageLabel->setWordWrap(true);
	messageLabel->setStyleSheet(messageLabelStyle);

	timerId = startTimer(1000);

	cancel();
}

void PopupMessage::show(QString message, int milliseconds)
{
	cancel();
	messageEndTime = QDateTime::currentDateTime().toMSecsSinceEpoch() + milliseconds;

	messageLabel->style()->unpolish(messageLabel);
	messageLabel->style()->polish(messageLabel);
	messageLabel->setText(message);
	messageLabel->adjustSize();
	int x = (messageLabel->parentWidget()->width() - messageLabel->width()) / 2;
	int y = messageLabel->parentWidget()->height() * 2 / 3 - messageLabel->height() / 2;
	messageLabel->move(x, y);
	messageLabel->raise();
	messageLabel->show();
}

void PopupMessage::cancel()
{
	messageEndTime = 0;
	messageLabel->hide();
}

void PopupMessage::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == timerId)
	{
		auto now = QDateTime::currentDateTime().toMSecsSinceEpoch();
		if (messageEndTime && now > messageEndTime)
			cancel();
	}
}
