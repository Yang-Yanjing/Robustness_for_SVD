static int config_props(AVFilterLink *inlink)
{
    AVFilterContext *ctx = inlink->dst;
    LutContext *s = ctx->priv;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    uint8_t rgba_map[4]; 
    int min[4], max[4];
    int val, color, ret;
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    s->var_values[VAR_W] = inlink->w;
    s->var_values[VAR_H] = inlink->h;
    s->is_16bit = desc->comp[0].depth_minus1 > 7;
    switch (inlink->format) {
    case AV_PIX_FMT_YUV410P:
    case AV_PIX_FMT_YUV411P:
    case AV_PIX_FMT_YUV420P:
    case AV_PIX_FMT_YUV422P:
    case AV_PIX_FMT_YUV440P:
    case AV_PIX_FMT_YUV444P:
    case AV_PIX_FMT_YUVA420P:
    case AV_PIX_FMT_YUVA422P:
    case AV_PIX_FMT_YUVA444P:
    case AV_PIX_FMT_YUV420P9LE:
    case AV_PIX_FMT_YUV422P9LE:
    case AV_PIX_FMT_YUV444P9LE:
    case AV_PIX_FMT_YUVA420P9LE:
    case AV_PIX_FMT_YUVA422P9LE:
    case AV_PIX_FMT_YUVA444P9LE:
    case AV_PIX_FMT_YUV420P10LE:
    case AV_PIX_FMT_YUV422P10LE:
    case AV_PIX_FMT_YUV440P10LE:
    case AV_PIX_FMT_YUV444P10LE:
    case AV_PIX_FMT_YUVA420P10LE:
    case AV_PIX_FMT_YUVA422P10LE:
    case AV_PIX_FMT_YUVA444P10LE:
    case AV_PIX_FMT_YUV420P12LE:
    case AV_PIX_FMT_YUV422P12LE:
    case AV_PIX_FMT_YUV440P12LE:
    case AV_PIX_FMT_YUV444P12LE:
    case AV_PIX_FMT_YUV420P14LE:
    case AV_PIX_FMT_YUV422P14LE:
    case AV_PIX_FMT_YUV444P14LE:
    case AV_PIX_FMT_YUV420P16LE:
    case AV_PIX_FMT_YUV422P16LE:
    case AV_PIX_FMT_YUV444P16LE:
    case AV_PIX_FMT_YUVA420P16LE:
    case AV_PIX_FMT_YUVA422P16LE:
    case AV_PIX_FMT_YUVA444P16LE:
        min[Y] = 16 * (1 << (desc->comp[0].depth_minus1 - 7));
        min[U] = 16 * (1 << (desc->comp[1].depth_minus1 - 7));
        min[V] = 16 * (1 << (desc->comp[2].depth_minus1 - 7));
        min[A] = 0;
        max[Y] = 235 * (1 << (desc->comp[0].depth_minus1 - 7));
        max[U] = 240 * (1 << (desc->comp[1].depth_minus1 - 7));
        max[V] = 240 * (1 << (desc->comp[2].depth_minus1 - 7));
        max[A] = (1 << (desc->comp[3].depth_minus1 + 1)) - 1;
        break;
    default:
        min[0] = min[1] = min[2] = min[3] = 0;
        max[0] = max[1] = max[2] = max[3] = 255;
    }
    s->is_yuv = s->is_rgb = 0;
    if      (ff_fmt_is_in(inlink->format, yuv_pix_fmts)) s->is_yuv = 1;
    else if (ff_fmt_is_in(inlink->format, rgb_pix_fmts)) s->is_rgb = 1;
    if (s->is_rgb) {
        ff_fill_rgba_map(rgba_map, inlink->format);
        s->step = av_get_bits_per_pixel(desc) >> 3;
    }
    for (color = 0; color < desc->nb_components; color++) {
        double res;
        int comp = s->is_rgb ? rgba_map[color] : color;
        
        av_expr_free(s->comp_expr[color]);
        s->comp_expr[color] = NULL;
        ret = av_expr_parse(&s->comp_expr[color], s->comp_expr_str[color],
                            var_names, funcs1_names, funcs1, NULL, NULL, 0, ctx);
        if (ret < 0) {
            av_log(ctx, AV_LOG_ERROR,
                   "Error when parsing the expression '%s' for the component %d and color %d.\n",
                   s->comp_expr_str[comp], comp, color);
            return AVERROR(EINVAL);
        }
        
        s->var_values[VAR_MAXVAL] = max[color];
        s->var_values[VAR_MINVAL] = min[color];
        for (val = 0; val < (1 << (desc->comp[0].depth_minus1 + 1)); val++) {
            s->var_values[VAR_VAL] = val;
            s->var_values[VAR_CLIPVAL] = av_clip(val, min[color], max[color]);
            s->var_values[VAR_NEGVAL] =
                av_clip(min[color] + max[color] - s->var_values[VAR_VAL],
                        min[color], max[color]);
            res = av_expr_eval(s->comp_expr[color], s->var_values, s);
            if (isnan(res)) {
                av_log(ctx, AV_LOG_ERROR,
                       "Error when evaluating the expression '%s' for the value %d for the component %d.\n",
                       s->comp_expr_str[color], val, comp);
                return AVERROR(EINVAL);
            }
            s->lut[comp][val] = av_clip((int)res, min[color], max[color]);
            av_log(ctx, AV_LOG_DEBUG, "val[%d][%d] = %d\n", comp, val, s->lut[comp][val]);
        }
    }
    return 0;
}
