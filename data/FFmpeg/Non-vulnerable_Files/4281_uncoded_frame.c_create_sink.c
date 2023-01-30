static int create_sink(Stream *st, AVFilterGraph *graph,
                       AVFilterContext *f, int idx)
{
    enum AVMediaType type = avfilter_pad_get_type(f->output_pads, idx);
    const char *sink_name;
    int ret;
    switch (type) {
    case AVMEDIA_TYPE_VIDEO: sink_name =  "buffersink"; break;
    case AVMEDIA_TYPE_AUDIO: sink_name = "abuffersink"; break;
    default:
        av_log(NULL, AV_LOG_ERROR, "Stream type not supported\n");
        return AVERROR(EINVAL);
    }
    ret = avfilter_graph_create_filter(&st->sink,
                                       avfilter_get_by_name(sink_name),
                                       NULL, NULL, NULL, graph);
    if (ret < 0)
        return ret;
    ret = avfilter_link(f, idx, st->sink, 0);
    if (ret < 0)
        return ret;
    st->link = st->sink->inputs[0];
    return 0;
}
