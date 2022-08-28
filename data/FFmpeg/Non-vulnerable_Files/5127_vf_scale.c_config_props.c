static int config_props(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink0 = outlink->src->inputs[0];
    AVFilterLink *inlink  = ctx->filter == &ff_vf_scale2ref ?
                            outlink->src->inputs[1] :
                            outlink->src->inputs[0];
    enum AVPixelFormat outfmt = outlink->format;
    ScaleContext *scale = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    const AVPixFmtDescriptor *out_desc = av_pix_fmt_desc_get(outlink->format);
    int64_t w, h;
    double var_values[VARS_NB], res;
    char *expr;
    int ret;
    int factor_w, factor_h;
    var_values[VAR_IN_W]  = var_values[VAR_IW] = inlink->w;
    var_values[VAR_IN_H]  = var_values[VAR_IH] = inlink->h;
    var_values[VAR_OUT_W] = var_values[VAR_OW] = NAN;
    var_values[VAR_OUT_H] = var_values[VAR_OH] = NAN;
    var_values[VAR_A]     = (double) inlink->w / inlink->h;
    var_values[VAR_SAR]   = inlink->sample_aspect_ratio.num ?
        (double) inlink->sample_aspect_ratio.num / inlink->sample_aspect_ratio.den : 1;
    var_values[VAR_DAR]   = var_values[VAR_A] * var_values[VAR_SAR];
    var_values[VAR_HSUB]  = 1 << desc->log2_chroma_w;
    var_values[VAR_VSUB]  = 1 << desc->log2_chroma_h;
    var_values[VAR_OHSUB] = 1 << out_desc->log2_chroma_w;
    var_values[VAR_OVSUB] = 1 << out_desc->log2_chroma_h;
    
    av_expr_parse_and_eval(&res, (expr = scale->w_expr),
                           var_names, var_values,
                           NULL, NULL, NULL, NULL, NULL, 0, ctx);
    scale->w = var_values[VAR_OUT_W] = var_values[VAR_OW] = res;
    if ((ret = av_expr_parse_and_eval(&res, (expr = scale->h_expr),
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto fail;
    scale->h = var_values[VAR_OUT_H] = var_values[VAR_OH] = res;
    
    if ((ret = av_expr_parse_and_eval(&res, (expr = scale->w_expr),
                                      var_names, var_values,
                                      NULL, NULL, NULL, NULL, NULL, 0, ctx)) < 0)
        goto fail;
    scale->w = res;
    w = scale->w;
    h = scale->h;
    

    factor_w = 1;
    factor_h = 1;
    if (w < -1) {
        factor_w = -w;
    }
    if (h < -1) {
        factor_h = -h;
    }
    if (w < 0 && h < 0)
        scale->w = scale->h = 0;
    if (!(w = scale->w))
        w = inlink->w;
    if (!(h = scale->h))
        h = inlink->h;
    


    if (w < 0)
        w = av_rescale(h, inlink->w, inlink->h * factor_w) * factor_w;
    if (h < 0)
        h = av_rescale(w, inlink->h, inlink->w * factor_h) * factor_h;
    

    if (scale->force_original_aspect_ratio) {
        int tmp_w = av_rescale(h, inlink->w, inlink->h);
        int tmp_h = av_rescale(w, inlink->h, inlink->w);
        if (scale->force_original_aspect_ratio == 1) {
             w = FFMIN(tmp_w, w);
             h = FFMIN(tmp_h, h);
        } else {
             w = FFMAX(tmp_w, w);
             h = FFMAX(tmp_h, h);
        }
    }
    if (w > INT_MAX || h > INT_MAX ||
        (h * inlink->w) > INT_MAX  ||
        (w * inlink->h) > INT_MAX)
        av_log(ctx, AV_LOG_ERROR, "Rescaled value for width or height is too big.\n");
    outlink->w = w;
    outlink->h = h;
    
    scale->input_is_pal = desc->flags & AV_PIX_FMT_FLAG_PAL ||
                          desc->flags & AV_PIX_FMT_FLAG_PSEUDOPAL;
    if (outfmt == AV_PIX_FMT_PAL8) outfmt = AV_PIX_FMT_BGR8;
    scale->output_is_pal = av_pix_fmt_desc_get(outfmt)->flags & AV_PIX_FMT_FLAG_PAL ||
                           av_pix_fmt_desc_get(outfmt)->flags & AV_PIX_FMT_FLAG_PSEUDOPAL;
    if (scale->sws)
        sws_freeContext(scale->sws);
    if (scale->isws[0])
        sws_freeContext(scale->isws[0]);
    if (scale->isws[1])
        sws_freeContext(scale->isws[1]);
    scale->isws[0] = scale->isws[1] = scale->sws = NULL;
    if (inlink0->w == outlink->w &&
        inlink0->h == outlink->h &&
        !scale->out_color_matrix &&
        scale->in_range == scale->out_range &&
        inlink0->format == outlink->format)
        ;
    else {
        struct SwsContext **swscs[3] = {&scale->sws, &scale->isws[0], &scale->isws[1]};
        int i;
        for (i = 0; i < 3; i++) {
            struct SwsContext **s = swscs[i];
            *s = sws_alloc_context();
            if (!*s)
                return AVERROR(ENOMEM);
            av_opt_set_int(*s, "srcw", inlink0 ->w, 0);
            av_opt_set_int(*s, "srch", inlink0 ->h >> !!i, 0);
            av_opt_set_int(*s, "src_format", inlink0->format, 0);
            av_opt_set_int(*s, "dstw", outlink->w, 0);
            av_opt_set_int(*s, "dsth", outlink->h >> !!i, 0);
            av_opt_set_int(*s, "dst_format", outfmt, 0);
            av_opt_set_int(*s, "sws_flags", scale->flags, 0);
            av_opt_set_int(*s, "param0", scale->param[0], 0);
            av_opt_set_int(*s, "param1", scale->param[1], 0);
            if (scale->in_range != AVCOL_RANGE_UNSPECIFIED)
                av_opt_set_int(*s, "src_range",
                               scale->in_range == AVCOL_RANGE_JPEG, 0);
            if (scale->out_range != AVCOL_RANGE_UNSPECIFIED)
                av_opt_set_int(*s, "dst_range",
                               scale->out_range == AVCOL_RANGE_JPEG, 0);
            if (scale->opts) {
                AVDictionaryEntry *e = NULL;
                while ((e = av_dict_get(scale->opts, "", e, AV_DICT_IGNORE_SUFFIX))) {
                    if ((ret = av_opt_set(*s, e->key, e->value, 0)) < 0)
                        return ret;
                }
            }
            


            if (inlink0->format == AV_PIX_FMT_YUV420P) {
                scale->in_v_chr_pos = (i == 0) ? 128 : (i == 1) ? 64 : 192;
            }
            if (outlink->format == AV_PIX_FMT_YUV420P) {
                scale->out_v_chr_pos = (i == 0) ? 128 : (i == 1) ? 64 : 192;
            }
            av_opt_set_int(*s, "src_h_chr_pos", scale->in_h_chr_pos, 0);
            av_opt_set_int(*s, "src_v_chr_pos", scale->in_v_chr_pos, 0);
            av_opt_set_int(*s, "dst_h_chr_pos", scale->out_h_chr_pos, 0);
            av_opt_set_int(*s, "dst_v_chr_pos", scale->out_v_chr_pos, 0);
            if ((ret = sws_init_context(*s, NULL, NULL)) < 0)
                return ret;
            if (!scale->interlaced)
                break;
        }
    }
    if (inlink->sample_aspect_ratio.num){
        outlink->sample_aspect_ratio = av_mul_q((AVRational){outlink->h * inlink->w, outlink->w * inlink->h}, inlink->sample_aspect_ratio);
    } else
        outlink->sample_aspect_ratio = inlink->sample_aspect_ratio;
    av_log(ctx, AV_LOG_VERBOSE, "w:%d h:%d fmt:%s sar:%d/%d -> w:%d h:%d fmt:%s sar:%d/%d flags:0x%0x\n",
           inlink ->w, inlink ->h, av_get_pix_fmt_name( inlink->format),
           inlink->sample_aspect_ratio.num, inlink->sample_aspect_ratio.den,
           outlink->w, outlink->h, av_get_pix_fmt_name(outlink->format),
           outlink->sample_aspect_ratio.num, outlink->sample_aspect_ratio.den,
           scale->flags);
    return 0;
fail:
    av_log(NULL, AV_LOG_ERROR,
           "Error when evaluating the expression '%s'.\n"
           "Maybe the expression for out_w:'%s' or for out_h:'%s' is self-referencing.\n",
           expr, scale->w_expr, scale->h_expr);
    return ret;
}
