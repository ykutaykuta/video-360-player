#include "ffmpegdecode.h"
//#include <QDebug>
//#include <QTime>

#define THRESHOLD_BUFF 150


FFMpegDecode::FFMpegDecode()
{
    mPtr_fmtCtx = nullptr;

    mPtr_videoCodecCtx = nullptr;
    mPtr_audioCodecCtx = nullptr;

    mPtr_frame = nullptr;
    mPtr_ARGBFrame = nullptr;
    mPtr_imageBuff = nullptr;

    mPtr_scaleCtx = nullptr;

    m_videoStreamIndex = -1;
    m_audioStreamIndex = -1;

	m_videoData = nullptr;

    m_videoFPS = 0.0;
	m_thresholdBuff = THRESHOLD_BUFF;
	isDecoding = false;
	isStopping = false;
}

FFMpegDecode::~FFMpegDecode()
{
	CloseFile();
}

void FFMpegDecode::setFilePath(const QString &tmp)
{
    m_filePath = tmp;
}

QString FFMpegDecode::getFilePath() const
{
    return m_filePath;
}

double FFMpegDecode::getFPS() const
{
    return m_videoFPS;
}

int FFMpegDecode::getAudioNbChannel() const
{
    return m_audioNbChannel;
}

int FFMpegDecode::getAudioNbSample() const
{
    return m_audioNbSample;
}

int FFMpegDecode::getAudioBytePerSample() const
{
    return m_audioBytePerSample;
}

int FFMpegDecode::getAudioSampleRate() const
{
    return m_audioSampleRate;
}

AVCodecContext *FFMpegDecode::getAudioCodecContext() const
{
    return mPtr_audioCodecCtx;
}

void FFMpegDecode::setThread(QThread *tmp)
{
	mPtr_thread = tmp;
	this->moveToThread(mPtr_thread);
	mPtr_thread = QThread::create(std::bind(&FFMpegDecode::GetNextFrame, this));

    mPtr_thread->start();
}

void FFMpegDecode::GetNextFrame()
{
	isEndDecoding = false;
    av_init_packet(&m_pkt);
    m_pkt.data = nullptr;
    m_pkt.size = 0;

    while(av_read_frame(mPtr_fmtCtx, &m_pkt) >= 0)
    {
		if(isStopping)
			break;
		while(mQ_videoQueueBuff.size()>m_thresholdBuff && mQ_audioQueueBuff.size()>m_thresholdBuff)
        {
			if(isStopping)
				break;
        }


		isDecoding = true;
        if(m_pkt.stream_index == m_videoStreamIndex)
        {
			if(DecodeVideoPacket(&got_frame) > 0)
            {
				if(got_frame > 0)
				{
					Convert2ARGBFrame();

					Frame tmp;
					tmp.linesize = mPtr_ARGBFrame->linesize[0];
					tmp.pts = mPtr_frame->pts;

					uint8_t *data = static_cast<uint8_t*>(av_malloc(m_videoSize));
					std::memcpy(data, mPtr_ARGBFrame->data[0], m_videoSize);

					tmp.data = data;


					mQ_videoQueueBuff.push_back(tmp);
				}
            }

            av_free_packet(&m_pkt);
			isDecoding = false;
            continue;
        }

        if(m_pkt.stream_index == m_audioStreamIndex)
        {
			if(DecodeAudioPacket(&got_frame) > 0)
            {
				if(got_frame > 0)
				{
					uint8_t *data = new uint8_t[m_audioSize];
					for(int i = 0; i<m_audioNbSample; i++)
					{
						for(int j = 0; j<m_audioNbChannel; j++)
						{
							memcpy(data + (i*m_audioNbChannel + j)*m_audioBytePerSample,
								   mPtr_frame->data[j] + i*m_audioBytePerSample, m_audioBytePerSample);
						}
					}

					Frame tmp;
					tmp.data = data;
					tmp.linesize = -1;
					tmp.pts = mPtr_frame->pts;

					mQ_audioQueueBuff.push_back(tmp);
				}
            }
            av_free_packet(&m_pkt);
			isDecoding = false;
        }
    }

	isEndDecoding = true;
}

bool FFMpegDecode::OpenFile()
{
    av_register_all();

    if(avformat_open_input(&mPtr_fmtCtx, m_filePath.toStdString().c_str(), nullptr, nullptr) < 0)
    {
        CloseFile();
//        qDebug()<<"Could open file";
        return false;
    }

    if(avformat_find_stream_info(mPtr_fmtCtx, nullptr) < 0)
    {
        CloseFile();
//        qDebug()<<"Could not find stream information";
        return false;
    }

    m_duration = mPtr_fmtCtx->duration;

    if(OpenCodecContext(&m_videoStreamIndex, AVMEDIA_TYPE_VIDEO) >= 0)
    {
        mPtr_videoStream = mPtr_fmtCtx->streams[m_videoStreamIndex];
        mPtr_videoCodecCtx = mPtr_videoStream->codec;

		m_videoTimeBase = av_q2d(mPtr_videoStream->time_base);

        m_width = mPtr_videoCodecCtx->width;
        m_height = mPtr_videoCodecCtx->height;

        m_videoFPS = av_q2d(mPtr_videoStream->r_frame_rate);
    }

    if(OpenCodecContext(&m_audioStreamIndex, AVMEDIA_TYPE_AUDIO) >= 0)
    {
        mPtr_audioStream = mPtr_fmtCtx->streams[m_audioStreamIndex];
        mPtr_audioCodecCtx = mPtr_audioStream->codec;

		m_audioTimeBase = av_q2d(mPtr_audioStream->time_base);

        m_audioNbChannel = mPtr_audioCodecCtx->channels;
        m_audioBytePerSample = av_get_bytes_per_sample((AVSampleFormat)mPtr_audioCodecCtx->sample_fmt);
        m_audioSampleRate = mPtr_audioCodecCtx->sample_rate;
        m_audioNbSample = mPtr_audioCodecCtx->frame_size;
		m_audioSize = m_audioNbSample * m_audioNbChannel * m_audioBytePerSample;
    }

    initFrame();

    return true;
}

void FFMpegDecode::CloseFile()
{
	isStopping = true;
	while(!isEndDecoding){

	}

    if(mPtr_audioCodecCtx)
        avcodec_close(mPtr_audioCodecCtx);

    if(mPtr_videoCodecCtx)
        avcodec_close(mPtr_videoCodecCtx);

    if(mPtr_fmtCtx)
    {
        avformat_close_input(&mPtr_fmtCtx);
    }

    freeFrame();
}

void FFMpegDecode::initFrame()
{   
    mPtr_frame = av_frame_alloc();
    if(!mPtr_frame)
    {
//        qDebug()<<"Could not allocate frame";
        return;
    }

    mPtr_ARGBFrame = av_frame_alloc();
    if(!mPtr_ARGBFrame)
    {
//        qDebug()<<"Could not allocate RGBA frame";
        return;
    }

	m_videoSize = avpicture_get_size(AV_PIX_FMT_RGB24, m_width, m_height);
	mPtr_imageBuff = static_cast<uint8_t*>(av_mallocz(m_videoSize));

    mPtr_scaleCtx = sws_getContext(m_width, m_height, mPtr_videoCodecCtx->pix_fmt, m_width, m_height,
                                   AV_PIX_FMT_RGB24, SWS_BICUBIC, nullptr, nullptr, nullptr);

    avpicture_fill((AVPicture*)mPtr_ARGBFrame, mPtr_imageBuff, AV_PIX_FMT_RGB24, m_width, m_height);

    mPtr_ARGBFrame->width = m_width;
    mPtr_ARGBFrame->height = m_height;
    mPtr_ARGBFrame->format = AVPixelFormat::AV_PIX_FMT_RGB24;

	m_videoData = new uint8_t[m_videoSize];
}

void FFMpegDecode::freeFrame()
{
    if(mPtr_frame)
        av_frame_free(&mPtr_frame);

    if(mPtr_imageBuff)
        av_free(mPtr_imageBuff);

    if(mPtr_ARGBFrame)
        av_frame_free(&mPtr_ARGBFrame);

    if(mPtr_scaleCtx)
        sws_freeContext(mPtr_scaleCtx);
}

int FFMpegDecode::OpenCodecContext(int *streamIndex, AVMediaType type)
{
    int ret;
    AVStream *stream;
    AVCodecContext *codecCtx = nullptr;
    AVCodec *codec = nullptr;
    ret = av_find_best_stream(mPtr_fmtCtx, type, -1, -1, nullptr, 0);
    if (ret < 0)
    {
//        qDebug()<<"Could find stream in input file";
    }else
    {
        *streamIndex = ret;
        stream = mPtr_fmtCtx->streams[*streamIndex];

        /* find decoder for the stream */
        codecCtx = stream->codec;
        codec = avcodec_find_decoder(codecCtx->codec_id);
        if (!codec)
        {
//            qDebug()<<"Failed to find "<<av_get_media_type_string(type)<<" codec";
            return ret;
        }
        if ((ret = avcodec_open2(codecCtx, codec, nullptr)) < 0)
        {
//            qDebug()<<"Failed to open "<<av_get_media_type_string(type)<<" codec";
            return ret;
        }
    }
    return 0;
}

int FFMpegDecode::DecodeVideoPacket(int *got_frame)
{
    int ret = avcodec_decode_video2(mPtr_videoCodecCtx, mPtr_frame, got_frame, &m_pkt);
    if(ret<0)
    {
//        qDebug()<<"Error decoding video frame";

    }
    return ret;
}

int FFMpegDecode::DecodeAudioPacket(int *got_frame)
{
    int ret = avcodec_decode_audio4(mPtr_audioCodecCtx, mPtr_frame, got_frame, &m_pkt);
    if(ret<0)
    {
//        qDebug()<<"Error decoding audio frame";

    }
    return ret;
}

void FFMpegDecode::Convert2ARGBFrame()
{
    sws_scale(mPtr_scaleCtx, mPtr_frame->data, mPtr_frame->linesize,
        0, m_height, mPtr_ARGBFrame->data, mPtr_ARGBFrame->linesize);
}


uint8_t *FFMpegDecode::DeQueueAudio()
{
    if(mQ_audioQueueBuff.size() == 0)
        return nullptr;
    Frame tmp = mQ_audioQueueBuff.takeFirst();
	m_currentTime = m_audioTimeBase * tmp.pts;

	int time = static_cast<int>(m_currentTime);
	if(time != m_currTime_int)
	{
		m_currTime_int = time;
		emit CurrTimeChanged(m_currTime_int);
	}

    return tmp.data;
}

#define TIME_SYNC_POS 0.1 // time in s
#define TIME_SYNC_NAV -0.1

QImage FFMpegDecode::DeQueueVideo()
{
    if(mQ_videoQueueBuff.size() == 0)
		return QImage();
    Frame tmp = mQ_videoQueueBuff.takeFirst();

	double diff = m_videoTimeBase * tmp.pts - m_currentTime;

	if(diff > TIME_SYNC_POS) // video is faster than audio
	{
		mQ_videoQueueBuff.push_front(tmp);
		QImage res = QImage(m_videoData, m_width, m_height, tmp.linesize, QImage::Format_RGB888);
		return  res;
	}

	if(diff < TIME_SYNC_NAV) // video is slowr than audio
	{
		av_free(tmp.data);

		if(mQ_videoQueueBuff.size() == 0)
			return QImage();
		tmp = mQ_videoQueueBuff.takeFirst();

		std::memcpy(m_videoData, tmp.data, m_videoSize);
		QImage res = QImage(m_videoData, m_width, m_height, tmp.linesize, QImage::Format_RGB888);
		av_free(tmp.data);

		return res;
	}

	std::memcpy(m_videoData, tmp.data, m_videoSize);
	QImage res = QImage(m_videoData, m_width, m_height, tmp.linesize, QImage::Format_RGB888);
	av_free(tmp.data);

	return res;

}

uint32_t FFMpegDecode::getAudioSize() const
{
	return m_audioSize;
}

int FFMpegDecode::getDuration() const
{
	return static_cast<int>(m_duration/1000000);
}

void FFMpegDecode::seekVideo(int pos)
{
	m_thresholdBuff = -1;
	while(isDecoding){

	}

	while(mQ_audioQueueBuff.size()>0)
	{
		Frame tmp = mQ_audioQueueBuff.takeFirst();
		delete[] tmp.data;
	}
	while(mQ_videoQueueBuff.size()>0)
	{
		Frame tmp = mQ_videoQueueBuff.takeFirst();
		av_free(tmp.data);
	}

	av_seek_frame(mPtr_fmtCtx, m_audioStreamIndex, static_cast<int64_t>(pos/m_audioTimeBase), AVSEEK_FLAG_ANY);
	avcodec_flush_buffers(mPtr_audioCodecCtx);
	av_seek_frame(mPtr_fmtCtx, m_videoStreamIndex, static_cast<int64_t>(pos/m_videoTimeBase), AVSEEK_FLAG_BACKWARD);
	avcodec_flush_buffers(mPtr_videoCodecCtx);

	m_thresholdBuff = THRESHOLD_BUFF;
}


