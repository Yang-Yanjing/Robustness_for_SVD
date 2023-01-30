static int create_subcc_streams(AVFormatContext *avctx)
{
    LavfiContext *lavfi = avctx->priv_data;
    AVStream *st;
    int stream_idx, sink_idx;
    for (stream_idx = 0; stream_idx < lavfi->nb_sinks; stream_idx++) {
        sink_idx = lavfi->stream_sink_map[stream_idx];
        if (lavfi->sink_stream_subcc_map[sink_idx]) {
            lavfi->sink_stream_subcc_map[sink_idx] = avctx->nb_streams;
            if (!(st = avformat_new_stream(avctx, NULL)))
                return AVERROR(ENOMEM);
            st->codec->codec_id = AV_CODEC_ID_EIA_608;
            st->codec->codec_type = AVMEDIA_TYPE_SUBTITLE;
        } else {
            lavfi->sink_stream_subcc_map[sink_idx] = -1;
        }
    }
    return 0;
}
