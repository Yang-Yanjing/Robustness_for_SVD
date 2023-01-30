void choose_sample_fmt(AVStream *st, AVCodec *codec)
{
    if (codec && codec->sample_fmts) {
        const enum AVSampleFormat *p = codec->sample_fmts;
        for (; *p != -1; p++) {
            if (*p == st->codecpar->format)
                break;
        }
        if (*p == -1) {
            if((codec->capabilities & AV_CODEC_CAP_LOSSLESS) && av_get_sample_fmt_name(st->codecpar->format) > av_get_sample_fmt_name(codec->sample_fmts[0]))
                av_log(NULL, AV_LOG_ERROR, "Conversion will not be lossless.\n");
            if(av_get_sample_fmt_name(st->codecpar->format))
            av_log(NULL, AV_LOG_WARNING,
                   "Incompatible sample format '%s' for codec '%s', auto-selecting format '%s'\n",
                   av_get_sample_fmt_name(st->codecpar->format),
                   codec->name,
                   av_get_sample_fmt_name(codec->sample_fmts[0]));
            st->codecpar->format = codec->sample_fmts[0];
        }
    }
}
