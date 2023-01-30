static int put_xiph_codecpriv(AVFormatContext *s, AVIOContext *pb, AVCodecContext *codec)
{
    const uint8_t *header_start[3];
    int header_len[3];
    int first_header_size;
    int j;
    if (codec->codec_id == AV_CODEC_ID_VORBIS)
        first_header_size = 30;
    else
        first_header_size = 42;
    if (avpriv_split_xiph_headers(codec->extradata, codec->extradata_size,
                              first_header_size, header_start, header_len) < 0) {
        av_log(s, AV_LOG_ERROR, "Extradata corrupt.\n");
        return -1;
    }
    avio_w8(pb, 2);                    
    for (j = 0; j < 2; j++) {
        put_xiph_size(pb, header_len[j]);
    }
    for (j = 0; j < 3; j++)
        avio_write(pb, header_start[j], header_len[j]);
    return 0;
}
