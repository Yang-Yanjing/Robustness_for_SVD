static void set_codec_str(AVCodecContext *codec, char *str, int size)
{
    switch (codec->codec_id) {
    case AV_CODEC_ID_H264:
        snprintf(str, size, "avc1");
        if (codec->extradata_size >= 4 && codec->extradata[0] == 1) {
            av_strlcatf(str, size, ".%02x%02x%02x",
                        codec->extradata[1], codec->extradata[2], codec->extradata[3]);
        }
        break;
    case AV_CODEC_ID_AAC:
        snprintf(str, size, "mp4a.40"); 
        if (codec->extradata_size >= 2) {
            int aot = codec->extradata[0] >> 3;
            if (aot == 31)
                aot = ((AV_RB16(codec->extradata) >> 5) & 0x3f) + 32;
            av_strlcatf(str, size, ".%d", aot);
        }
        break;
    }
}
