#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include "ffmpegdecode.h"
//#include <QDebug>

#if defined(__WIN32__)
#define SDL_MAIN_HANDLED
#endif

#include "SDL.h"

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();

    void initPlayer(FFMpegDecode *tmp);
	void destroyAll();
    static void addData(void *udata, Uint8 *stream, int len);

    void setSampleRate(const int tmp);
    void setSampleSize(const int tmp);
    void setNbChannel(const int tmp);
    void setThread(QThread *tmp);
    void setLenghtFrame(const int tmp);
	void setVolume(int &tmp);


signals:
    void addBuff();

public slots:
    void getBuff();
	void play();
	void pause();

private:



    uint8_t m_nbChannel;
    int m_sampleRate;
    int m_sampleSize;
    int m_lenghtFrame;
    uint16_t m_nbSample;

    static uint32_t m_audioLen;
    static uint8_t* mPtr_audioChuck;

    static AudioOutput *mPtr_instance;


    SDL_AudioSpec m_audioSpec;

    QThread *mPtr_thread;
    FFMpegDecode *mPtr_decode;

	static uint8_t m_volume; // 0-128 default 70

};

#endif // AUDIOOUTPUT_H
