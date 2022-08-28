static av_cold int audio_write_header(AVFormatContext *s1)
{
    SndioData *s = s1->priv_data;
    AVStream *st;
    int ret;
    st             = s1->streams[0];
    s->sample_rate = st->codec->sample_rate;
    s->channels    = st->codec->channels;
    ret = ff_sndio_open(s1, 1, s1->filename);
    return ret;
}
