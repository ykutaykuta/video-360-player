#ifndef VIDEOOUTPUT_H
#define VIDEOOUTPUT_H

#include <QObject>
#include <QThread>
#include <QTimer>
#include "ffmpegdecode.h"
#include "openglqml.h"

class VideoOutput : public QObject
{
    Q_OBJECT
public:
    explicit VideoOutput(QObject *parent = nullptr);
    ~VideoOutput();

    void setThread(QThread *tmp, int delay);
    void initPlayer(FFMpegDecode *tmp);
	Q_INVOKABLE void destroyAll();

public slots:
    void onTimeOut();
	void PlayPause();

private:
    FFMpegDecode *mPtr_decode;
    QThread *mPtr_thread;
	QTimer *mPtr_timer;
	int m_delay;
};

#endif // VIDEOOUTPUT_H
