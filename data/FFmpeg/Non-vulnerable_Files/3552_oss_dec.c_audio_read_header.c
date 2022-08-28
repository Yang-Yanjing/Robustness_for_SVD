static int audio_read_header(AVFormatContext *s1)
{
    OSSAudioData *s = s1->priv_data;
    AVStream *st;
    int ret;
    st = avformat_new_stream(s1, NULL);
    if (!st) {
        return AVERROR(ENOMEM);
    }
    ret = ff_oss_audio_open(s1, 0, s1->filename);
    if (ret < 0) {
        return AVERROR(EIO);
    }
    
    st->codec->codec_type = AVMEDIA_TYPE_AUDIO;
    st->codec->codec_id = s->codec_id;
    st->codec->sample_rate = s->sample_rate;
    st->codec->channels = s->channels;
    avpriv_set_pts_info(st, 64, 1, 1000000);  
    return 0;
}
