static int filter_frame(AVFilterLink *inlink, AVFrame *frame)
{
    int ret;
    AVFilterContext *ctx = inlink->dst;
    PermsContext *perms = ctx->priv;
    AVFrame *out = frame;
    enum perm in_perm = av_frame_is_writable(frame) ? RW : RO;
    enum perm out_perm;
    switch (perms->mode) {
    case MODE_TOGGLE:   out_perm = in_perm == RO ? RW : RO;                 break;
    case MODE_RANDOM:   out_perm = av_lfg_get(&perms->lfg) & 1 ? RW : RO;   break;
    case MODE_RO:       out_perm = RO;                                      break;
    case MODE_RW:       out_perm = RW;                                      break;
    default:            out_perm = in_perm;                                 break;
    }
    av_log(ctx, AV_LOG_VERBOSE, "%s -> %s%s\n",
           perm_str[in_perm], perm_str[out_perm],
           in_perm == out_perm ? " (no-op)" : "");
    if (in_perm == RO && out_perm == RW) {
        if ((ret = av_frame_make_writable(frame)) < 0)
            return ret;
    } else if (in_perm == RW && out_perm == RO) {
        out = av_frame_clone(frame);
        if (!out)
            return AVERROR(ENOMEM);
    }
    ret = ff_filter_frame(ctx->outputs[0], out);
    if (in_perm == RW && out_perm == RO)
        av_frame_free(&frame);
    return ret;
}
