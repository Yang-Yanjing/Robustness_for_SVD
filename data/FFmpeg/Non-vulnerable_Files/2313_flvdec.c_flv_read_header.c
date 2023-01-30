static int flv_read_header(AVFormatContext *s)
{
    int offset, flags;
    avio_skip(s->pb, 4);
    flags = avio_r8(s->pb);
    s->ctx_flags |= AVFMTCTX_NOHEADER;
    if (flags & FLV_HEADER_FLAG_HASVIDEO)
        if (!create_stream(s, AVMEDIA_TYPE_VIDEO))
            return AVERROR(ENOMEM);
    if (flags & FLV_HEADER_FLAG_HASAUDIO)
        if (!create_stream(s, AVMEDIA_TYPE_AUDIO))
            return AVERROR(ENOMEM);
    
    
    offset = avio_rb32(s->pb);
    avio_seek(s->pb, offset, SEEK_SET);
    avio_skip(s->pb, 4);
    s->start_time = 0;
    return 0;
}
