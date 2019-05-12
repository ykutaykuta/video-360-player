#include "videooutput.h"

VideoOutput::VideoOutput(QObject *parent) : QObject(parent)
{
    mPtr_thread = nullptr;
	mPtr_timer = new QTimer();
}

VideoOutput::~VideoOutput()
{
	QMetaObject::invokeMethod(this, "destroyAll");
}

void VideoOutput::setThread(QThread *tmp, int delay)
{
	m_delay = delay;
	mPtr_timer->setInterval(m_delay);
	mPtr_timer->setTimerType(Qt::TimerType::PreciseTimer);
    mPtr_thread = tmp;

	QObject::connect(mPtr_timer, SIGNAL(timeout()), this, SLOT(onTimeOut()), Qt::DirectConnection);
	QObject::connect(mPtr_thread, SIGNAL(started()), mPtr_timer, SLOT(start()), Qt::DirectConnection);

    this->moveToThread(mPtr_thread);
	mPtr_timer->moveToThread(mPtr_thread);
	mPtr_thread->msleep(500);

    mPtr_thread->start();


}

void VideoOutput::initPlayer(FFMpegDecode *tmp)
{
	mPtr_decode = tmp;
}

void VideoOutput::destroyAll()
{
	mPtr_timer->stop();
}

void VideoOutput::PlayPause()
{
	if(mPtr_timer->interval() == m_delay)
	{
		mPtr_timer->start(INT32_MAX);
	}else
	{
		mPtr_timer->start(m_delay);
	}
}

void VideoOutput::onTimeOut()
{
    QImage tmp = mPtr_decode->DeQueueVideo();
	if(tmp.isNull())
		return;
    OpenGLQmlRenderer::setTexture(tmp);
}
