static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *toplink = ctx->inputs[TOP];
    BlendContext *s = ctx->priv;
    const AVPixFmtDescriptor *pix_desc = av_pix_fmt_desc_get(toplink->format);
    int ret, plane, is_16bit;
    if (!s->tblend) {
        AVFilterLink *bottomlink = ctx->inputs[BOTTOM];
        if (toplink->format != bottomlink->format) {
            av_log(ctx, AV_LOG_ERROR, "inputs must be of same pixel format\n");
            return AVERROR(EINVAL);
        }
        if (toplink->w                       != bottomlink->w ||
            toplink->h                       != bottomlink->h ||
            toplink->sample_aspect_ratio.num != bottomlink->sample_aspect_ratio.num ||
            toplink->sample_aspect_ratio.den != bottomlink->sample_aspect_ratio.den) {
            av_log(ctx, AV_LOG_ERROR, "First input link %s parameters "
                   "(size %dx%d, SAR %d:%d) do not match the corresponding "
                   "second input link %s parameters (%dx%d, SAR %d:%d)\n",
                   ctx->input_pads[TOP].name, toplink->w, toplink->h,
                   toplink->sample_aspect_ratio.num,
                   toplink->sample_aspect_ratio.den,
                   ctx->input_pads[BOTTOM].name, bottomlink->w, bottomlink->h,
                   bottomlink->sample_aspect_ratio.num,
                   bottomlink->sample_aspect_ratio.den);
            return AVERROR(EINVAL);
        }
    }
    outlink->w = toplink->w;
    outlink->h = toplink->h;
    outlink->time_base = toplink->time_base;
    outlink->sample_aspect_ratio = toplink->sample_aspect_ratio;
    outlink->frame_rate = toplink->frame_rate;
    s->hsub = pix_desc->log2_chroma_w;
    s->vsub = pix_desc->log2_chroma_h;
    is_16bit = pix_desc->comp[0].depth_minus1 == 15;
    s->nb_planes = av_pix_fmt_count_planes(toplink->format);
    if (s->tblend)
        outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
    else if ((ret = ff_dualinput_init(ctx, &s->dinput)) < 0)
            return ret;
    for (plane = 0; plane < FF_ARRAY_ELEMS(s->params); plane++) {
        FilterParams *param = &s->params[plane];
        if (s->all_mode >= 0)
            param->mode = s->all_mode;
        if (s->all_opacity < 1)
            param->opacity = s->all_opacity;
        switch (param->mode) {
        case BLEND_ADDITION:   param->blend = is_16bit ? blend_addition_16bit   : blend_addition_8bit;   break;
        case BLEND_AND:        param->blend = is_16bit ? blend_and_16bit        : blend_and_8bit;        break;
        case BLEND_AVERAGE:    param->blend = is_16bit ? blend_average_16bit    : blend_average_8bit;    break;
        case BLEND_BURN:       param->blend = is_16bit ? blend_burn_16bit       : blend_burn_8bit;       break;
        case BLEND_DARKEN:     param->blend = is_16bit ? blend_darken_16bit     : blend_darken_8bit;     break;
        case BLEND_DIFFERENCE: param->blend = is_16bit ? blend_difference_16bit : blend_difference_8bit; break;
        case BLEND_DIFFERENCE128: param->blend = is_16bit ? blend_difference128_16bit: blend_difference128_8bit; break;
        case BLEND_DIVIDE:     param->blend = is_16bit ? blend_divide_16bit     : blend_divide_8bit;     break;
        case BLEND_DODGE:      param->blend = is_16bit ? blend_dodge_16bit      : blend_dodge_8bit;      break;
        case BLEND_EXCLUSION:  param->blend = is_16bit ? blend_exclusion_16bit  : blend_exclusion_8bit;  break;
        case BLEND_GLOW:       param->blend = is_16bit ? blend_glow_16bit       : blend_glow_8bit;       break;
        case BLEND_HARDLIGHT:  param->blend = is_16bit ? blend_hardlight_16bit  : blend_hardlight_8bit;  break;
        case BLEND_HARDMIX:    param->blend = is_16bit ? blend_hardmix_16bit    : blend_hardmix_8bit;    break;
        case BLEND_LIGHTEN:    param->blend = is_16bit ? blend_lighten_16bit    : blend_lighten_8bit;    break;
        case BLEND_LINEARLIGHT:param->blend = is_16bit ? blend_linearlight_16bit: blend_linearlight_8bit;break;
        case BLEND_MULTIPLY:   param->blend = is_16bit ? blend_multiply_16bit   : blend_multiply_8bit;   break;
        case BLEND_NEGATION:   param->blend = is_16bit ? blend_negation_16bit   : blend_negation_8bit;   break;
        case BLEND_NORMAL:     param->blend = blend_normal;                                              break;
        case BLEND_OR:         param->blend = is_16bit ? blend_or_16bit         : blend_or_8bit;         break;
        case BLEND_OVERLAY:    param->blend = is_16bit ? blend_overlay_16bit    : blend_overlay_8bit;    break;
        case BLEND_PHOENIX:    param->blend = is_16bit ? blend_phoenix_16bit    : blend_phoenix_8bit;    break;
        case BLEND_PINLIGHT:   param->blend = is_16bit ? blend_pinlight_16bit   : blend_pinlight_8bit;   break;
        case BLEND_REFLECT:    param->blend = is_16bit ? blend_reflect_16bit    : blend_reflect_8bit;    break;
        case BLEND_SCREEN:     param->blend = is_16bit ? blend_screen_16bit     : blend_screen_8bit;     break;
        case BLEND_SOFTLIGHT:  param->blend = is_16bit ? blend_softlight_16bit  : blend_softlight_8bit;  break;
        case BLEND_SUBTRACT:   param->blend = is_16bit ? blend_subtract_16bit   : blend_subtract_8bit;   break;
        case BLEND_VIVIDLIGHT: param->blend = is_16bit ? blend_vividlight_16bit : blend_vividlight_8bit; break;
        case BLEND_XOR:        param->blend = is_16bit ? blend_xor_16bit        : blend_xor_8bit;        break;
        }
        if (s->all_expr && !param->expr_str) {
            param->expr_str = av_strdup(s->all_expr);
            if (!param->expr_str)
                return AVERROR(ENOMEM);
        }
        if (param->expr_str) {
            ret = av_expr_parse(&param->e, param->expr_str, var_names,
                                NULL, NULL, NULL, NULL, 0, ctx);
            if (ret < 0)
                return ret;
            param->blend = is_16bit? blend_expr_16bit : blend_expr_8bit;
        }
    }
    return 0;
}
