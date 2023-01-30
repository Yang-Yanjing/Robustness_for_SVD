static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    StabData *sd = ctx->priv;
    VSMotionDetect* md = &(sd->md);
    VSFrameInfo fi;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    vsFrameInfoInit(&fi, inlink->w, inlink->h,
                    ff_av2vs_pixfmt(ctx, inlink->format));
    if (fi.bytesPerPixel != av_get_bits_per_pixel(desc)/8) {
        av_log(ctx, AV_LOG_ERROR, "pixel-format error: wrong bits/per/pixel, please report a BUG");
        return AVERROR(EINVAL);
    }
    if (fi.log2ChromaW != desc->log2_chroma_w) {
        av_log(ctx, AV_LOG_ERROR, "pixel-format error: log2_chroma_w, please report a BUG");
        return AVERROR(EINVAL);
    }
    if (fi.log2ChromaH != desc->log2_chroma_h) {
        av_log(ctx, AV_LOG_ERROR, "pixel-format error: log2_chroma_h, please report a BUG");
        return AVERROR(EINVAL);
    }
    
    sd->conf.algo     = 1;
    sd->conf.modName  = "vidstabdetect";
    if (vsMotionDetectInit(md, &sd->conf, &fi) != VS_OK) {
        av_log(ctx, AV_LOG_ERROR, "initialization of Motion Detection failed, please report a BUG");
        return AVERROR(EINVAL);
    }
    vsMotionDetectGetConfig(&sd->conf, md);
    av_log(ctx, AV_LOG_INFO, "Video stabilization settings (pass 1/2):\n");
    av_log(ctx, AV_LOG_INFO, "     shakiness = %d\n", sd->conf.shakiness);
    av_log(ctx, AV_LOG_INFO, "      accuracy = %d\n", sd->conf.accuracy);
    av_log(ctx, AV_LOG_INFO, "      stepsize = %d\n", sd->conf.stepSize);
    av_log(ctx, AV_LOG_INFO, "   mincontrast = %f\n", sd->conf.contrastThreshold);
    av_log(ctx, AV_LOG_INFO, "        tripod = %d\n", sd->conf.virtualTripod);
    av_log(ctx, AV_LOG_INFO, "          show = %d\n", sd->conf.show);
    av_log(ctx, AV_LOG_INFO, "        result = %s\n", sd->result);
    sd->f = fopen(sd->result, "w");
    if (sd->f == NULL) {
        av_log(ctx, AV_LOG_ERROR, "cannot open transform file %s\n", sd->result);
        return AVERROR(EINVAL);
    } else {
        if (vsPrepareFile(md, sd->f) != VS_OK) {
            av_log(ctx, AV_LOG_ERROR, "cannot write to transform file %s\n", sd->result);
            return AVERROR(EINVAL);
        }
    }
    return 0;
}
