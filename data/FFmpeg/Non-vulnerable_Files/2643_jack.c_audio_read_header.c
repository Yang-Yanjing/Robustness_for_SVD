static int audio_read_header(AVFormatContext *context)
{
    JackData *self = context->priv_data;
    AVStream *stream;
    int test;
    if ((test = start_jack(context)))
        return test;
    stream = avformat_new_stream(context, NULL);
    if (!stream) {
        stop_jack(self);
        return AVERROR(ENOMEM);
    }
    stream->codec->codec_type   = AVMEDIA_TYPE_AUDIO;
#if HAVE_BIGENDIAN
    stream->codec->codec_id     = AV_CODEC_ID_PCM_F32BE;
#else
    stream->codec->codec_id     = AV_CODEC_ID_PCM_F32LE;
#endif
    stream->codec->sample_rate  = self->sample_rate;
    stream->codec->channels     = self->nports;
    avpriv_set_pts_info(stream, 64, 1, 1000000);  
    return 0;
}
