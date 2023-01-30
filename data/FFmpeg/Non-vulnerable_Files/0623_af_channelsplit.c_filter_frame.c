static int filter_frame(AVFilterLink *inlink, AVFrame *buf)
{
    AVFilterContext *ctx = inlink->dst;
    int i, ret = 0;
    for (i = 0; i < ctx->nb_outputs; i++) {
        AVFrame *buf_out = av_frame_clone(buf);
        if (!buf_out) {
            ret = AVERROR(ENOMEM);
            break;
        }
        buf_out->data[0] = buf_out->extended_data[0] = buf_out->extended_data[i];
        buf_out->channel_layout =
            av_channel_layout_extract_channel(buf->channel_layout, i);
        av_frame_set_channels(buf_out, 1);
        ret = ff_filter_frame(ctx->outputs[i], buf_out);
        if (ret < 0)
            break;
    }
    av_frame_free(&buf);
    return ret;
}
