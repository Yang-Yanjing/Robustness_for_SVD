int avfilter_link(AVFilterContext *src, unsigned srcpad,
                  AVFilterContext *dst, unsigned dstpad)
{
    AVFilterLink *link;
    if (src->nb_outputs <= srcpad || dst->nb_inputs <= dstpad ||
        src->outputs[srcpad]      || dst->inputs[dstpad])
        return AVERROR(EINVAL);
    if (src->output_pads[srcpad].type != dst->input_pads[dstpad].type) {
        av_log(src, AV_LOG_ERROR,
               "Media type mismatch between the '%s' filter output pad %d (%s) and the '%s' filter input pad %d (%s)\n",
               src->name, srcpad, (char *)av_x_if_null(av_get_media_type_string(src->output_pads[srcpad].type), "?"),
               dst->name, dstpad, (char *)av_x_if_null(av_get_media_type_string(dst-> input_pads[dstpad].type), "?"));
        return AVERROR(EINVAL);
    }
    link = av_mallocz(sizeof(*link));
    if (!link)
        return AVERROR(ENOMEM);
    src->outputs[srcpad] = dst->inputs[dstpad] = link;
    link->src     = src;
    link->dst     = dst;
    link->srcpad  = &src->output_pads[srcpad];
    link->dstpad  = &dst->input_pads[dstpad];
    link->type    = src->output_pads[srcpad].type;
    av_assert0(AV_PIX_FMT_NONE == -1 && AV_SAMPLE_FMT_NONE == -1);
    link->format  = -1;
    return 0;
}
