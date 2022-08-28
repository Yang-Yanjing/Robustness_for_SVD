av_cold static int lavfi_read_close(AVFormatContext *avctx)
{
    LavfiContext *lavfi = avctx->priv_data;
    av_freep(&lavfi->sink_stream_map);
    av_freep(&lavfi->sink_eof);
    av_freep(&lavfi->stream_sink_map);
    av_freep(&lavfi->sink_stream_subcc_map);
    av_freep(&lavfi->sinks);
    avfilter_graph_free(&lavfi->graph);
    av_frame_free(&lavfi->decoded_frame);
    return 0;
}
