AVFILTER_DEFINE_CLASS(lut3d);
static av_cold int lut3d_init(AVFilterContext *ctx)
{
    int ret;
    FILE *f;
    const char *ext;
    LUT3DContext *lut3d = ctx->priv;
    if (!lut3d->file) {
        set_identity_matrix(lut3d, 32);
        return 0;
    }
    f = fopen(lut3d->file, "r");
    if (!f) {
        ret = AVERROR(errno);
        av_log(ctx, AV_LOG_ERROR, "%s: %s\n", lut3d->file, av_err2str(ret));
        return ret;
    }
    ext = strrchr(lut3d->file, '.');
    if (!ext) {
        av_log(ctx, AV_LOG_ERROR, "Unable to guess the format from the extension\n");
        ret = AVERROR_INVALIDDATA;
        goto end;
    }
    ext++;
    if (!av_strcasecmp(ext, "dat")) {
        ret = parse_dat(ctx, f);
    } else if (!av_strcasecmp(ext, "3dl")) {
        ret = parse_3dl(ctx, f);
    } else if (!av_strcasecmp(ext, "cube")) {
        ret = parse_cube(ctx, f);
    } else if (!av_strcasecmp(ext, "m3d")) {
        ret = parse_m3d(ctx, f);
    } else {
        av_log(ctx, AV_LOG_ERROR, "Unrecognized '.%s' file type\n", ext);
        ret = AVERROR(EINVAL);
    }
    if (!ret && !lut3d->lutsize) {
        av_log(ctx, AV_LOG_ERROR, "3D LUT is empty\n");
        ret = AVERROR_INVALIDDATA;
    }
end:
    fclose(f);
    return ret;
}
