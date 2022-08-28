static av_cold int init(AVFilterContext *ctx)
{
    int ret;
    DeshakeContext *deshake = ctx->priv;
    deshake->sad = av_pixelutils_get_sad_fn(4, 4, 1, deshake); 
    if (!deshake->sad)
        return AVERROR(EINVAL);
    deshake->refcount = 20; 
    deshake->blocksize /= 2;
    deshake->blocksize = av_clip(deshake->blocksize, 4, 128);
    if (deshake->rx % 16) {
        av_log(ctx, AV_LOG_ERROR, "rx must be a multiple of 16\n");
        return AVERROR_PATCHWELCOME;
    }
    if (deshake->filename)
        deshake->fp = fopen(deshake->filename, "w");
    if (deshake->fp)
        fwrite("Ori x, Avg x, Fin x, Ori y, Avg y, Fin y, Ori angle, Avg angle, Fin angle, Ori zoom, Avg zoom, Fin zoom\n", sizeof(char), 104, deshake->fp);
    
    
    if (deshake->cx > 0) {
        deshake->cw += deshake->cx - (deshake->cx & ~15);
        deshake->cx &= ~15;
    }
    deshake->transform = deshake_transform_c;
    if (!CONFIG_OPENCL && deshake->opencl) {
        av_log(ctx, AV_LOG_ERROR, "OpenCL support was not enabled in this build, cannot be selected\n");
        return AVERROR(EINVAL);
    }
    if (CONFIG_OPENCL && deshake->opencl) {
        deshake->transform = ff_opencl_transform;
        ret = ff_opencl_deshake_init(ctx);
        if (ret < 0)
            return ret;
    }
    av_log(ctx, AV_LOG_VERBOSE, "cx: %d, cy: %d, cw: %d, ch: %d, rx: %d, ry: %d, edge: %d blocksize: %d contrast: %d search: %d\n",
           deshake->cx, deshake->cy, deshake->cw, deshake->ch,
           deshake->rx, deshake->ry, deshake->edge, deshake->blocksize * 2, deshake->contrast, deshake->search);
    return 0;
}
