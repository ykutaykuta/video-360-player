#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QWidget>
#include <QFileDialog>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "videohandle.h"

class Controller : public QWidget
{
	Q_OBJECT

	Q_PROPERTY(int CurrTime READ CurrTime WRITE setCurrTime NOTIFY currTimeChanged)
	Q_PROPERTY(int Duration READ Duration WRITE setDuration NOTIFY durationChanged)
public:
	explicit Controller(QQmlApplicationEngine *eng, QWidget *parent = nullptr);

	Q_INVOKABLE void openFileDialog();
	Q_INVOKABLE void playVideo() const;
	Q_INVOKABLE void pauseVideo() const;
	Q_INVOKABLE void setVolume(const int &tmp);
	Q_INVOKABLE void seekVideo(const int &pos);

	int CurrTime() const;
	int Duration() const;
	void setCurrTime(const int &tmp);
	void setDuration(const int &tmp);


signals:
	void currTimeChanged();
	void durationChanged();

public slots:
	void setCurrTime_slot();
	void setDuration_slot();

private:
	QString m_filePath;
	QQmlApplicationEngine *mPtr_eng;
	VideoHandle *mPtr_videoHandle;
};

#endif // MYCLASS_H
