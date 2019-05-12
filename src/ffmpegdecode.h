#ifndef FFMPEGDECODE_H
#define FFMPEGDECODE_H

#include <QString>
#include <QImage>
#include <cstring>

extern "C"{
#include <libavutil/imgutils.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/opt.h>
}

#include <QList>
#include <QThread>

typedef struct Frame
{
    uint8_t *data;
    int linesize;
    int64_t pts;
} Frame;


class FFMpegDecode : public QObject
{
    Q_OBJECT
public:
    FFMpegDecode();
    ~FFMpegDecode();

	void setFilePath(const QString &tmp);
    QString getFilePath()   const;
    double getFPS() const;
    int getAudioNbChannel() const;
    int getAudioNbSample() const;
    int getAudioBytePerSample() const;
    int getAudioSampleRate() const;

    AVCodecContext *getAudioCodecContext() const;

    void setThread(QThread *tmp);

    bool OpenFile();
    void CloseFile();

    void initFrame();
    void freeFrame();

    int OpenCodecContext(int *streamIndex, enum AVMediaType type);


    void Convert2ARGBFrame();

    uint8_t *DeQueueAudio();
    QImage DeQueueVideo();
	uint32_t getAudioSize() const;

	int getDuration() const;

	void seekVideo(int pos);

signals:
	void CurrTimeChanged(int);

public slots:
	void GetNextFrame();

private:
	int DecodeVideoPacket(int *got_frame);
	int DecodeAudioPacket(int *got_frame);

private:
    QString m_filePath;

    AVStream *mPtr_videoStream;
    AVStream *mPtr_audioStream;

    AVFrame *mPtr_frame;
    AVFrame *mPtr_ARGBFrame;
    uint8_t *mPtr_imageBuff;
    AVPacket m_pkt;

    AVFormatContext *mPtr_fmtCtx;

    AVCodecContext *mPtr_videoCodecCtx;
    AVCodecContext *mPtr_audioCodecCtx;

    int m_videoStreamIndex;
    int m_audioStreamIndex;

    double m_videoFPS;

    SwsContext *mPtr_scaleCtx;

    int m_width;
    int m_height;
	int got_frame;
	int64_t m_duration;
	double m_audioTimeBase;
	double m_videoTimeBase;

	double m_currentTime;
	int m_currTime_int;


    int m_audioOutBuffeSize;

    int m_audioNbChannel;
    int m_audioNbSample; // number sample per channel
    int m_audioBytePerSample;
    int m_audioSampleRate;
	uint32_t m_audioSize;
	uint32_t m_videoSize;

    QList<Frame> mQ_audioQueueBuff;
    QList<Frame> mQ_videoQueueBuff;
	uint8_t *m_videoData;
	int m_thresholdBuff;

    QThread *mPtr_thread;

	bool isDecoding;
	bool isStopping;
	bool isEndDecoding;
};

#endif // FFMPEGDECODE_H
