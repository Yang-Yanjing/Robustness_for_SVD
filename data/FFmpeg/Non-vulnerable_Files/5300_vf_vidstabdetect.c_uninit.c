static av_cold void uninit(AVFilterContext *ctx)
{
    StabData *sd = ctx->priv;
    VSMotionDetect *md = &(sd->md);
    if (sd->f) {
        fclose(sd->f);
        sd->f = NULL;
    }
    vsMotionDetectionCleanup(md);
}
