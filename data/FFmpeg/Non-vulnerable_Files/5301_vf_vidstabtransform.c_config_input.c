static int config_input(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    TransformContext *tc = ctx->priv;
    FILE *f;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    VSTransformData *td = &(tc->td);
    VSFrameInfo fi_src;
    VSFrameInfo fi_dest;
    if (!vsFrameInfoInit(&fi_src, inlink->w, inlink->h,
                         ff_av2vs_pixfmt(ctx, inlink->format)) ||
        !vsFrameInfoInit(&fi_dest, inlink->w, inlink->h,
                         ff_av2vs_pixfmt(ctx, inlink->format))) {
        av_log(ctx, AV_LOG_ERROR, "unknown pixel format: %i (%s)",
               inlink->format, desc->name);
        return AVERROR(EINVAL);
    }
    if (fi_src.bytesPerPixel != av_get_bits_per_pixel(desc)/8 ||
        fi_src.log2ChromaW != desc->log2_chroma_w ||
        fi_src.log2ChromaH != desc->log2_chroma_h) {
        av_log(ctx, AV_LOG_ERROR, "pixel-format error: bpp %i<>%i  ",
               fi_src.bytesPerPixel, av_get_bits_per_pixel(desc)/8);
        av_log(ctx, AV_LOG_ERROR, "chroma_subsampl: w: %i<>%i  h: %i<>%i\n",
               fi_src.log2ChromaW, desc->log2_chroma_w,
               fi_src.log2ChromaH, desc->log2_chroma_h);
        return AVERROR(EINVAL);
    }
    
    tc->conf.modName = "vidstabtransform";
    tc->conf.verbose = 1 + tc->debug;
    if (tc->tripod) {
        av_log(ctx, AV_LOG_INFO, "Virtual tripod mode: relative=0, smoothing=0\n");
        tc->conf.relative  = 0;
        tc->conf.smoothing = 0;
    }
    tc->conf.simpleMotionCalculation = 0;
    tc->conf.storeTransforms         = tc->debug;
    tc->conf.smoothZoom              = 0;
    if (vsTransformDataInit(td, &tc->conf, &fi_src, &fi_dest) != VS_OK) {
        av_log(ctx, AV_LOG_ERROR, "initialization of vid.stab transform failed, please report a BUG\n");
        return AVERROR(EINVAL);
    }
    vsTransformGetConfig(&tc->conf, td);
    av_log(ctx, AV_LOG_INFO, "Video transformation/stabilization settings (pass 2/2):\n");
    av_log(ctx, AV_LOG_INFO, "    input     = %s\n", tc->input);
    av_log(ctx, AV_LOG_INFO, "    smoothing = %d\n", tc->conf.smoothing);
    av_log(ctx, AV_LOG_INFO, "    optalgo   = %s\n",
           tc->conf.camPathAlgo == VSOptimalL1 ? "opt" :
           (tc->conf.camPathAlgo == VSGaussian ? "gauss" : "avg"));
    av_log(ctx, AV_LOG_INFO, "    maxshift  = %d\n", tc->conf.maxShift);
    av_log(ctx, AV_LOG_INFO, "    maxangle  = %f\n", tc->conf.maxAngle);
    av_log(ctx, AV_LOG_INFO, "    crop      = %s\n", tc->conf.crop ? "Black" : "Keep");
    av_log(ctx, AV_LOG_INFO, "    relative  = %s\n", tc->conf.relative ? "True": "False");
    av_log(ctx, AV_LOG_INFO, "    invert    = %s\n", tc->conf.invert ? "True" : "False");
    av_log(ctx, AV_LOG_INFO, "    zoom      = %f\n", tc->conf.zoom);
    av_log(ctx, AV_LOG_INFO, "    optzoom   = %s\n",
           tc->conf.optZoom == 1 ? "Static (1)" : (tc->conf.optZoom == 2 ? "Dynamic (2)" : "Off (0)"));
    if (tc->conf.optZoom == 2)
        av_log(ctx, AV_LOG_INFO, "    zoomspeed = %g\n", tc->conf.zoomSpeed);
    av_log(ctx, AV_LOG_INFO, "    interpol  = %s\n", getInterpolationTypeName(tc->conf.interpolType));
    f = fopen(tc->input, "r");
    if (!f) {
        int ret = AVERROR(errno);
        av_log(ctx, AV_LOG_ERROR, "cannot open input file %s\n", tc->input);
        return ret;
    } else {
        VSManyLocalMotions mlms;
        if (vsReadLocalMotionsFile(f, &mlms) == VS_OK) {
            
            if (vsLocalmotions2Transforms(td, &mlms, &tc->trans) != VS_OK) {
                av_log(ctx, AV_LOG_ERROR, "calculating transformations failed\n");
                return AVERROR(EINVAL);
            }
        } else { 
            if (!vsReadOldTransforms(td, f, &tc->trans)) { 
                av_log(ctx, AV_LOG_ERROR, "error parsing input file %s\n", tc->input);
                return AVERROR(EINVAL);
            }
        }
    }
    fclose(f);
    if (vsPreprocessTransforms(td, &tc->trans) != VS_OK) {
        av_log(ctx, AV_LOG_ERROR, "error while preprocessing transforms\n");
        return AVERROR(EINVAL);
    }
    
    return 0;
}
