static int mkv_query_codec(enum AVCodecID codec_id, int std_compliance)
{
    int i;
    for (i = 0; ff_mkv_codec_tags[i].id != AV_CODEC_ID_NONE; i++)
        if (ff_mkv_codec_tags[i].id == codec_id)
            return 1;
    if (std_compliance < FF_COMPLIANCE_NORMAL) {
        enum AVMediaType type = avcodec_get_type(codec_id);
        
        if (type == AVMEDIA_TYPE_VIDEO || type == AVMEDIA_TYPE_AUDIO)
            return 1;
    }
    return 0;
}
