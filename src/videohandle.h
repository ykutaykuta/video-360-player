#ifndef VIDEOHANDLE_H
#define VIDEOHANDLE_H

#include <QObject>
#include "openglqml.h"
#include "ffmpegdecode.h"
#include "audiooutput.h"
#include "videooutput.h"
#include <QThread>
#include <QQmlApplicationEngine>
#include <QQmlContext>

class VideoHandle : public QObject
{
    Q_OBJECT

	Q_PROPERTY(int CurrTime READ CurrTime WRITE setCurrTime NOTIFY currTimeChanged)
	Q_PROPERTY(int Duration READ Duration WRITE setDuration NOTIFY durationChanged)
public:
	explicit VideoHandle(QQmlApplicationEngine *eng, QObject *parent = nullptr);
	~VideoHandle();

    double getFPS() const;
    void setFPS(const double tmp);

    void Decode();
    void initPlayer();

	int CurrTime() const;
	void setCurrTime(const int &tmp);

	int Duration() const;
	void setDuration(const int &tmp);

	Q_INVOKABLE void setVolume(int tmp);
	Q_INVOKABLE void seekVideo(int pos);
	Q_INVOKABLE void playVideo();
	Q_INVOKABLE void pauseVideo();
	Q_INVOKABLE void destroyAll();


public slots:
	void setCurrTime_slot(int tmp);
	Q_INVOKABLE void init(const QString &path);

signals:
	void currTimeChanged();
	void durationChanged();
	void Play();
	void Pause();

private:
    FFMpegDecode *mPtr_ffmpeg;
    AudioOutput *mPtr_audioOutput;
    VideoOutput *mPtr_videoOutput;

    double m_videoFPS;

    QThread m_thread_decode;
    QThread m_thread_audio;
    QThread m_thread_video;

	QQmlApplicationEngine *mPtr_eng;

	int m_currTime;
	int m_duration;
	bool m_isPlaying;
};

#endif // VIDEOHANDLE_H
