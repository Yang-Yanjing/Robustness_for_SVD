static void dump_unknown(AVFilterContext *ctx, AVFrameSideData *sd)
{
    av_log(ctx, AV_LOG_INFO, "unknown side data type: %d, size %d bytes", sd->type, sd->size);
}
