#include "audiooutput.h"

uint32_t AudioOutput::m_audioLen = 0;
uint8_t* AudioOutput::mPtr_audioChuck = nullptr;
uint8_t AudioOutput::m_volume = 70;

AudioOutput* AudioOutput::mPtr_instance = nullptr;

AudioOutput::AudioOutput(QObject *parent) : QObject(parent)
{
    // default format

    mPtr_thread = nullptr;
	mPtr_instance = this;
	mPtr_audioChuck = nullptr;

    QObject::connect(this, SIGNAL(addBuff()), this, SLOT(getBuff()), Qt::DirectConnection);
}

AudioOutput::~AudioOutput()
{
	destroyAll();
}

void AudioOutput::initPlayer(FFMpegDecode *tmp)
{
    mPtr_decode = tmp;

    if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
//        qDebug()<<"Could not initialize SDL";
        return;
    }

    m_sampleRate = mPtr_decode->getAudioSampleRate();
    m_nbChannel = mPtr_decode->getAudioNbChannel();
    m_nbSample = mPtr_decode->getAudioNbSample();

    m_audioSpec.freq = m_sampleRate;

    m_audioSpec.format = AUDIO_F32;
    m_audioSpec.channels = m_nbChannel;

    m_audioSpec.silence = 0;
    m_audioSpec.samples = m_nbSample;

    m_audioSpec.callback = addData;
    m_audioSpec.userdata = mPtr_decode->getAudioCodecContext();

	m_audioLen = mPtr_decode->getAudioSize();

    if (SDL_OpenAudio(&m_audioSpec, nullptr)<0)
    {
//        qDebug()<<"can't open audio";
        return;
    }
}

void AudioOutput::destroyAll()
{
    SDL_CloseAudio();
    SDL_Quit();
	if(mPtr_audioChuck != nullptr)
	{
		delete[] mPtr_audioChuck;
		mPtr_audioChuck = nullptr;
	}
}

void AudioOutput::addData(void *udata, Uint8 *stream, int len)
{
    if(mPtr_audioChuck != nullptr)
    {
        SDL_memset(stream, 0, m_audioLen);
		SDL_MixAudio(stream, mPtr_audioChuck, m_audioLen, m_volume);
    }

    emit mPtr_instance->addBuff();
}

void AudioOutput::setSampleRate(const int tmp)
{
    m_sampleRate = tmp;
}

void AudioOutput::setSampleSize(const int tmp)
{
    m_sampleSize = tmp;
}

void AudioOutput::setNbChannel(const int tmp)
{
    m_nbChannel = tmp;
}


void AudioOutput::setThread(QThread *tmp)
{
    mPtr_thread = tmp;
    this->moveToThread(mPtr_thread);
	mPtr_thread->msleep(500);
    mPtr_thread->start();

    SDL_PauseAudio(0);
}

void AudioOutput::setLenghtFrame(const int tmp)
{
	m_lenghtFrame = tmp;
}

void AudioOutput::setVolume(int &tmp)
{
	m_volume = static_cast<uint8_t>(tmp);
}

void AudioOutput::play()
{
	SDL_PauseAudio(0);
}

void AudioOutput::pause()
{
	SDL_PauseAudio(1);
}

void AudioOutput::getBuff()
{
    if(mPtr_audioChuck != nullptr)
    {
        delete[] mPtr_audioChuck;
    }
    mPtr_audioChuck = mPtr_decode->DeQueueAudio();

}

