static int write_streamheader(AVFormatContext *avctx, AVIOContext *bc,
                              AVStream *st, int i)
{
    NUTContext *nut       = avctx->priv_data;
    AVCodecContext *codec = st->codec;
    ff_put_v(bc, i);
    switch (codec->codec_type) {
    case AVMEDIA_TYPE_VIDEO:    ff_put_v(bc, 0); break;
    case AVMEDIA_TYPE_AUDIO:    ff_put_v(bc, 1); break;
    case AVMEDIA_TYPE_SUBTITLE: ff_put_v(bc, 2); break;
    default:                    ff_put_v(bc, 3); break;
    }
    ff_put_v(bc, 4);
    if (codec->codec_tag) {
        avio_wl32(bc, codec->codec_tag);
    } else {
        av_log(avctx, AV_LOG_ERROR, "No codec tag defined for stream %d\n", i);
        return AVERROR(EINVAL);
    }
    ff_put_v(bc, nut->stream[i].time_base - nut->time_base);
    ff_put_v(bc, nut->stream[i].msb_pts_shift);
    ff_put_v(bc, nut->stream[i].max_pts_distance);
    ff_put_v(bc, codec->has_b_frames);
    avio_w8(bc, 0); 
    ff_put_v(bc, codec->extradata_size);
    avio_write(bc, codec->extradata, codec->extradata_size);
    switch (codec->codec_type) {
    case AVMEDIA_TYPE_AUDIO:
        ff_put_v(bc, codec->sample_rate);
        ff_put_v(bc, 1);
        ff_put_v(bc, codec->channels);
        break;
    case AVMEDIA_TYPE_VIDEO:
        ff_put_v(bc, codec->width);
        ff_put_v(bc, codec->height);
        if (st->sample_aspect_ratio.num <= 0 ||
            st->sample_aspect_ratio.den <= 0) {
            ff_put_v(bc, 0);
            ff_put_v(bc, 0);
        } else {
            ff_put_v(bc, st->sample_aspect_ratio.num);
            ff_put_v(bc, st->sample_aspect_ratio.den);
        }
        ff_put_v(bc, 0); 
        break;
    default:
        break;
    }
    return 0;
}
