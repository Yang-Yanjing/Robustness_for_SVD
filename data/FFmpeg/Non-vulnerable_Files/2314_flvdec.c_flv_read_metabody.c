static int flv_read_metabody(AVFormatContext *s, int64_t next_pos)
{
    AMFDataType type;
    AVStream *stream, *astream, *vstream;
    AVStream av_unused *dstream;
    AVIOContext *ioc;
    int i;
    
    
    char buffer[32];
    astream = NULL;
    vstream = NULL;
    dstream = NULL;
    ioc     = s->pb;
    
    type = avio_r8(ioc);
    if (type != AMF_DATA_TYPE_STRING ||
        amf_get_string(ioc, buffer, sizeof(buffer)) < 0)
        return TYPE_UNKNOWN;
    if (!strcmp(buffer, "onTextData"))
        return TYPE_ONTEXTDATA;
    if (!strcmp(buffer, "onCaption"))
        return TYPE_ONCAPTION;
    if (!strcmp(buffer, "onCaptionInfo"))
        return TYPE_ONCAPTIONINFO;
    if (strcmp(buffer, "onMetaData") && strcmp(buffer, "onCuePoint")) {
        av_log(s, AV_LOG_DEBUG, "Unknown type %s\n", buffer);
        return TYPE_UNKNOWN;
    }
    
    
    for (i = 0; i < s->nb_streams; i++) {
        stream = s->streams[i];
        if (stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
            vstream = stream;
        else if (stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
            astream = stream;
        else if (stream->codec->codec_type == AVMEDIA_TYPE_SUBTITLE)
            dstream = stream;
    }
    
    if (amf_parse_object(s, astream, vstream, buffer, next_pos, 0) < 0)
        return -1;
    return 0;
}
