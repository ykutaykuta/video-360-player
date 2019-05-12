#include "videohandle.h"
//#include <QDebug>
//#include <QDir>

VideoHandle::VideoHandle(QQmlApplicationEngine *eng, QObject *parent) : QObject(parent)
{
	mPtr_eng = eng;
	mPtr_eng->rootContext()->setContextProperty("VideoHandle", this);
}

VideoHandle::~VideoHandle()
{
	destroyAll();
}

double VideoHandle::getFPS() const
{
    return m_videoFPS;
}

void VideoHandle::setFPS(const double tmp)
{
    m_videoFPS = tmp;
}

void VideoHandle::Decode()
{
    mPtr_ffmpeg->setThread(&m_thread_decode);
}

void VideoHandle::initPlayer()
{
    mPtr_audioOutput = new AudioOutput();
    mPtr_audioOutput->setNbChannel(mPtr_ffmpeg->getAudioNbChannel());
    mPtr_audioOutput->setSampleRate(mPtr_ffmpeg->getAudioSampleRate());
    mPtr_audioOutput->setSampleSize(mPtr_ffmpeg->getAudioBytePerSample());
    mPtr_audioOutput->setLenghtFrame(mPtr_ffmpeg->getAudioNbChannel()*
                                     mPtr_ffmpeg->getAudioBytePerSample()*
                                     mPtr_ffmpeg->getAudioNbSample());

    mPtr_videoOutput = new VideoOutput();
//    int delay = static_cast<int>(1000.0/mPtr_ffmpeg->getFPS()+0.5);

    mPtr_audioOutput->initPlayer(mPtr_ffmpeg);
    mPtr_videoOutput->initPlayer(mPtr_ffmpeg);

    mPtr_audioOutput->setThread(&m_thread_audio);
	mPtr_videoOutput->setThread(&m_thread_video, 20);

	QObject::connect(this, &VideoHandle::Play, mPtr_audioOutput, &AudioOutput::play);
	QObject::connect(this, &VideoHandle::Play, mPtr_videoOutput, &VideoOutput::PlayPause);

	QObject::connect(this, &VideoHandle::Pause, mPtr_audioOutput, &AudioOutput::pause);
	QObject::connect(this, &VideoHandle::Pause, mPtr_videoOutput, &VideoOutput::PlayPause);
}

int VideoHandle::CurrTime() const
{
	return m_currTime;
}

void VideoHandle::setCurrTime(const int &tmp)
{
	m_currTime = tmp;
	emit currTimeChanged();
}

int VideoHandle::Duration() const
{
	return m_duration;
}

void VideoHandle::setDuration(const int &tmp)
{
	m_duration = tmp;
	emit durationChanged();
}

void VideoHandle::setVolume(int tmp)
{
	mPtr_audioOutput->setVolume(tmp);
}

void VideoHandle::seekVideo(int pos)
{
//	int diff = pos - m_currTime;
	pauseVideo();
	mPtr_ffmpeg->seekVideo(pos);
	playVideo();
}

void VideoHandle::playVideo()
{
	emit Play();
}

void VideoHandle::pauseVideo()
{
	emit Pause();
}

void VideoHandle::destroyAll()
{
	m_thread_video.exit();
	m_thread_audio.exit();
	m_thread_decode.exit();

	if(mPtr_videoOutput != nullptr)
	{
		delete mPtr_videoOutput;
		mPtr_videoOutput = nullptr;
	}

	if(mPtr_audioOutput != nullptr)
	{
		delete mPtr_audioOutput;
		mPtr_audioOutput = nullptr;
	}

	if(mPtr_ffmpeg != nullptr)
	{
		delete mPtr_ffmpeg;
		mPtr_ffmpeg = nullptr;
	}
}

void VideoHandle::setCurrTime_slot(int tmp)
{
	setCurrTime(tmp);
}

void VideoHandle::init(const QString &path)
{
	mPtr_ffmpeg = new FFMpegDecode();
	QObject::connect(mPtr_ffmpeg, &FFMpegDecode::CurrTimeChanged, this, &VideoHandle::setCurrTime_slot, Qt::DirectConnection);

	mPtr_ffmpeg->setFilePath(path);
	mPtr_ffmpeg->OpenFile();
	setDuration(mPtr_ffmpeg->getDuration());

	Decode();
	initPlayer();
}
