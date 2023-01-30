static int parse_video_info(AVIOContext *pb, AVStream *st)
{
    uint16_t size;
    unsigned int tag;
    st->codec->width  = avio_rl32(pb);
    st->codec->height = avio_rl32(pb);
    avio_skip(pb, 1); 
    size = avio_rl16(pb); 
    tag  = ff_get_bmp_header(pb, st, NULL);
    st->codec->codec_tag = tag;
    st->codec->codec_id  = ff_codec_get_id(ff_codec_bmp_tags, tag);
    if (size > BMP_HEADER_SIZE) {
        int ret;
        st->codec->extradata_size  = size - BMP_HEADER_SIZE;
        if (!(st->codec->extradata = av_malloc(st->codec->extradata_size +
                                               AV_INPUT_BUFFER_PADDING_SIZE))) {
            st->codec->extradata_size = 0;
            return AVERROR(ENOMEM);
        }
        memset(st->codec->extradata + st->codec->extradata_size , 0,
               AV_INPUT_BUFFER_PADDING_SIZE);
        if ((ret = avio_read(pb, st->codec->extradata,
                             st->codec->extradata_size)) < 0)
            return ret;
    }
    return 0;
}
