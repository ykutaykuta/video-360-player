#include "controller.h"

Controller::Controller(QQmlApplicationEngine *eng, QWidget *parent) : QWidget(parent)
{
	mPtr_eng = eng;
	mPtr_eng->rootContext()->setContextProperty("Controller", this);
	mPtr_videoHandle = nullptr;
}

void Controller::openFileDialog()
{
	m_filePath = QFileDialog::getOpenFileName(this, "Open Video", "C:\\",
											  "Video Files (*.mp4 *.avi *.3gp *.flv *m4v *.amv *.mkv *.webm *.wmv)");
	if(m_filePath != "")
	{
		if(mPtr_videoHandle != nullptr)
		{
			delete mPtr_videoHandle;
			mPtr_videoHandle = nullptr;
		}

		mPtr_videoHandle = new VideoHandle(mPtr_eng);

		connect(mPtr_videoHandle, SIGNAL(currTimeChanged()), this, SLOT(setCurrTime_slot()));
		connect(mPtr_videoHandle, SIGNAL(durationChanged()), this, SLOT(setDuration_slot()));

		mPtr_videoHandle->init(m_filePath);
	}
}

void Controller::playVideo() const
{
	if(mPtr_videoHandle != nullptr)
		mPtr_videoHandle->playVideo();
}

void Controller::pauseVideo() const
{
	if(mPtr_videoHandle != nullptr)
		mPtr_videoHandle->pauseVideo();
}

void Controller::setVolume(const int &tmp)
{
	if(mPtr_videoHandle != nullptr)
	{
		mPtr_videoHandle->setVolume(tmp);
	}
}

void Controller::seekVideo(const int &pos)
{
	if(mPtr_videoHandle != nullptr)
	{
		mPtr_videoHandle->seekVideo(pos);
	}
}

int Controller::CurrTime() const
{
	if(mPtr_videoHandle == nullptr)
		return 0;
	return mPtr_videoHandle->CurrTime();
}

void Controller::setCurrTime_slot()
{
	emit currTimeChanged();
}

int Controller::Duration() const
{
	if(mPtr_videoHandle == nullptr)
		return 0;
	return mPtr_videoHandle->Duration();
}

void Controller::setCurrTime(const int &tmp)
{
	if(mPtr_videoHandle != nullptr)
	{
		mPtr_videoHandle->setCurrTime(tmp);
		emit currTimeChanged();
	}
}

void Controller::setDuration(const int &tmp)
{
	if(mPtr_videoHandle != nullptr)
	{
		mPtr_videoHandle->setDuration(tmp);
		emit durationChanged();
	}
}

void Controller::setDuration_slot()
{
	emit durationChanged();
}
