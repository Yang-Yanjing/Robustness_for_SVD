static int return_frame(AVFilterContext *ctx, int is_second)
{
    YADIFContext *yadif = ctx->priv;
    AVFilterLink *link  = ctx->outputs[0];
    int tff, ret;
    if (yadif->parity == -1) {
        tff = yadif->cur->interlaced_frame ?
              yadif->cur->top_field_first : 1;
    } else {
        tff = yadif->parity ^ 1;
    }
    if (is_second) {
        yadif->out = ff_get_video_buffer(link, link->w, link->h);
        if (!yadif->out)
            return AVERROR(ENOMEM);
        av_frame_copy_props(yadif->out, yadif->cur);
        yadif->out->interlaced_frame = 0;
    }
    filter(ctx, yadif->out, tff ^ !is_second, tff);
    if (is_second) {
        int64_t cur_pts  = yadif->cur->pts;
        int64_t next_pts = yadif->next->pts;
        if (next_pts != AV_NOPTS_VALUE && cur_pts != AV_NOPTS_VALUE) {
            yadif->out->pts = cur_pts + next_pts;
        } else {
            yadif->out->pts = AV_NOPTS_VALUE;
        }
    }
    ret = ff_filter_frame(ctx->outputs[0], yadif->out);
    yadif->frame_pending = (yadif->mode&1) && !is_second;
    return ret;
}
