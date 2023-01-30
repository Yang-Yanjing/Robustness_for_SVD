static AVStream *create_stream(AVFormatContext *s, int codec_type)
{
    AVStream *st = avformat_new_stream(s, NULL);
    if (!st)
        return NULL;
    st->codec->codec_type = codec_type;
    if (s->nb_streams>=3 ||(   s->nb_streams==2
                           && s->streams[0]->codec->codec_type != AVMEDIA_TYPE_SUBTITLE
                           && s->streams[1]->codec->codec_type != AVMEDIA_TYPE_SUBTITLE))
        s->ctx_flags &= ~AVFMTCTX_NOHEADER;
    avpriv_set_pts_info(st, 32, 1, 1000); 
    return st;
}
