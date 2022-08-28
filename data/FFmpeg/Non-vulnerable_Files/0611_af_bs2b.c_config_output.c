static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    Bs2bContext    *bs2b = ctx->priv;
    AVFilterLink *inlink = ctx->inputs[0];
    int srate = inlink->sample_rate;
    switch (inlink->format) {
    case AV_SAMPLE_FMT_U8:
        bs2b->filter = bs2b_cross_feed_u8;
        break;
    case AV_SAMPLE_FMT_S16:
        bs2b->filter = (void*)bs2b_cross_feed_s16;
        break;
    case AV_SAMPLE_FMT_S32:
        bs2b->filter = (void*)bs2b_cross_feed_s32;
        break;
    case AV_SAMPLE_FMT_FLT:
        bs2b->filter = (void*)bs2b_cross_feed_f;
        break;
    case AV_SAMPLE_FMT_DBL:
        bs2b->filter = (void*)bs2b_cross_feed_d;
        break;
    default:
        return AVERROR_BUG;
    }
    if ((srate < BS2B_MINSRATE) || (srate > BS2B_MAXSRATE))
        return AVERROR(ENOSYS);
    bs2b_set_srate(bs2b->bs2bp, srate);
    return 0;
}
