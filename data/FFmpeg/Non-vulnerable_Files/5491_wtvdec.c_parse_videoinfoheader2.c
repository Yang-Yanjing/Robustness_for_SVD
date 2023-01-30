static int parse_videoinfoheader2(AVFormatContext *s, AVStream *st)
{
    WtvContext *wtv = s->priv_data;
    AVIOContext *pb = wtv->pb;
    avio_skip(pb, 72);  
    st->codec->codec_tag = ff_get_bmp_header(pb, st, NULL);
    return 72 + 40;
}
