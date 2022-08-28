static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    AVFilterLink *inlink = ctx->inputs[0];
    Stereo3DContext *s = ctx->priv;
    AVRational aspect = inlink->sample_aspect_ratio;
    AVRational fps = inlink->frame_rate;
    AVRational tb = inlink->time_base;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(outlink->format);
    int ret;
    switch (s->in.format) {
    case SIDE_BY_SIDE_2_LR:
    case SIDE_BY_SIDE_LR:
    case SIDE_BY_SIDE_2_RL:
    case SIDE_BY_SIDE_RL:
        if (inlink->w & 1) {
            av_log(ctx, AV_LOG_ERROR, "width must be even\n");
            return AVERROR_INVALIDDATA;
        }
        break;
    case ABOVE_BELOW_2_LR:
    case ABOVE_BELOW_LR:
    case ABOVE_BELOW_2_RL:
    case ABOVE_BELOW_RL:
        if (s->out.format == INTERLEAVE_ROWS_LR ||
            s->out.format == INTERLEAVE_ROWS_RL) {
            if (inlink->h & 3) {
                av_log(ctx, AV_LOG_ERROR, "height must be multiple of 4\n");
                return AVERROR_INVALIDDATA;
            }
        }
        if (inlink->h & 1) {
            av_log(ctx, AV_LOG_ERROR, "height must be even\n");
            return AVERROR_INVALIDDATA;
        }
        break;
    }
    s->in.width     =
    s->width        = inlink->w;
    s->in.height    =
    s->height       = inlink->h;
    s->row_step     = 1;
    s->in.off_lstep =
    s->in.off_rstep =
    s->in.off_left  =
    s->in.off_right =
    s->in.row_left  =
    s->in.row_right = 0;
    switch (s->in.format) {
    case SIDE_BY_SIDE_2_LR:
        aspect.num     *= 2;
    case SIDE_BY_SIDE_LR:
        s->width        = inlink->w / 2;
        s->in.off_right = s->width;
        break;
    case SIDE_BY_SIDE_2_RL:
        aspect.num     *= 2;
    case SIDE_BY_SIDE_RL:
        s->width        = inlink->w / 2;
        s->in.off_left  = s->width;
        break;
    case ABOVE_BELOW_2_LR:
        aspect.den     *= 2;
    case ABOVE_BELOW_LR:
        s->in.row_right =
        s->height       = inlink->h / 2;
        break;
    case ABOVE_BELOW_2_RL:
        aspect.den     *= 2;
    case ABOVE_BELOW_RL:
        s->in.row_left  =
        s->height       = inlink->h / 2;
        break;
    case ALTERNATING_RL:
    case ALTERNATING_LR:
        outlink->flags |= FF_LINK_FLAG_REQUEST_LOOP;
        fps.den        *= 2;
        tb.num         *= 2;
        break;
    default:
        av_log(ctx, AV_LOG_ERROR, "input format %d is not supported\n", s->in.format);
        return AVERROR(EINVAL);
    }
    s->out.width     = s->width;
    s->out.height    = s->height;
    s->out.off_lstep =
    s->out.off_rstep =
    s->out.off_left  =
    s->out.off_right =
    s->out.row_left  =
    s->out.row_right = 0;
    switch (s->out.format) {
    case ANAGLYPH_RB_GRAY:
    case ANAGLYPH_RG_GRAY:
    case ANAGLYPH_RC_GRAY:
    case ANAGLYPH_RC_HALF:
    case ANAGLYPH_RC_COLOR:
    case ANAGLYPH_RC_DUBOIS:
    case ANAGLYPH_GM_GRAY:
    case ANAGLYPH_GM_HALF:
    case ANAGLYPH_GM_COLOR:
    case ANAGLYPH_GM_DUBOIS:
    case ANAGLYPH_YB_GRAY:
    case ANAGLYPH_YB_HALF:
    case ANAGLYPH_YB_COLOR:
    case ANAGLYPH_YB_DUBOIS: {
        uint8_t rgba_map[4];
        ff_fill_rgba_map(rgba_map, outlink->format);
        s->ana_matrix[rgba_map[0]] = &ana_coeff[s->out.format][0][0];
        s->ana_matrix[rgba_map[1]] = &ana_coeff[s->out.format][1][0];
        s->ana_matrix[rgba_map[2]] = &ana_coeff[s->out.format][2][0];
        break;
    }
    case SIDE_BY_SIDE_2_LR:
        aspect.den      *= 2;
    case SIDE_BY_SIDE_LR:
        s->out.width     = s->width * 2;
        s->out.off_right = s->width;
        break;
    case SIDE_BY_SIDE_2_RL:
        aspect.den      *= 2;
    case SIDE_BY_SIDE_RL:
        s->out.width     = s->width * 2;
        s->out.off_left  = s->width;
        break;
    case ABOVE_BELOW_2_LR:
        aspect.num      *= 2;
    case ABOVE_BELOW_LR:
        s->out.height    = s->height * 2;
        s->out.row_right = s->height;
        break;
    case ABOVE_BELOW_2_RL:
        aspect.num      *= 2;
    case ABOVE_BELOW_RL:
        s->out.height    = s->height * 2;
        s->out.row_left  = s->height;
        break;
    case INTERLEAVE_ROWS_LR:
        s->row_step      = 2;
        s->height        = s->height / 2;
        s->out.off_rstep =
        s->in.off_rstep  = 1;
        break;
    case INTERLEAVE_ROWS_RL:
        s->row_step      = 2;
        s->height        = s->height / 2;
        s->out.off_lstep =
        s->in.off_lstep  = 1;
        break;
    case MONO_R:
        s->in.off_left   = s->in.off_right;
        s->in.row_left   = s->in.row_right;
    case MONO_L:
        break;
    case ALTERNATING_RL:
    case ALTERNATING_LR:
        fps.num         *= 2;
        tb.den          *= 2;
        break;
    default:
        av_log(ctx, AV_LOG_ERROR, "output format %d is not supported\n", s->out.format);
        return AVERROR(EINVAL);
    }
    outlink->w = s->out.width;
    outlink->h = s->out.height;
    outlink->frame_rate = fps;
    outlink->time_base = tb;
    outlink->sample_aspect_ratio = aspect;
    if ((ret = av_image_fill_linesizes(s->linesize, outlink->format, s->width)) < 0)
        return ret;
    s->nb_planes = av_pix_fmt_count_planes(outlink->format);
    av_image_fill_max_pixsteps(s->pixstep, NULL, desc);
    s->ts_unit = av_q2d(av_inv_q(av_mul_q(outlink->frame_rate, outlink->time_base)));
    s->pheight[1] = s->pheight[2] = FF_CEIL_RSHIFT(s->height, desc->log2_chroma_h);
    s->pheight[0] = s->pheight[3] = s->height;
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    return 0;
}
