static int filter_frame(AVFilterLink *inlink, AVFrame *inpicref)
{
    AVFilterContext *ctx  = inlink->dst;
    Stereo3DContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out, *oleft, *oright, *ileft, *iright;
    int out_off_left[4], out_off_right[4];
    int i;
    switch (s->in.format) {
    case ALTERNATING_LR:
    case ALTERNATING_RL:
        if (!s->prev) {
            s->prev = inpicref;
            return 0;
        }
        ileft  = s->prev;
        iright = inpicref;
        if (s->in.format == ALTERNATING_RL)
            FFSWAP(AVFrame *, ileft, iright);
        break;
    default:
        ileft = iright = inpicref;
    };
    out = oleft = oright = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&s->prev);
        av_frame_free(&inpicref);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, inpicref);
    if (s->out.format == ALTERNATING_LR ||
        s->out.format == ALTERNATING_RL) {
        oright = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        if (!oright) {
            av_frame_free(&oleft);
            av_frame_free(&s->prev);
            av_frame_free(&inpicref);
            return AVERROR(ENOMEM);
        }
        av_frame_copy_props(oright, inpicref);
    }
    for (i = 0; i < 4; i++) {
        int hsub = i == 1 || i == 2 ? s->hsub : 0;
        int vsub = i == 1 || i == 2 ? s->vsub : 0;
        s->in_off_left[i]   = (FF_CEIL_RSHIFT(s->in.row_left,   vsub) + s->in.off_lstep)  * ileft->linesize[i]  + FF_CEIL_RSHIFT(s->in.off_left   * s->pixstep[i], hsub);
        s->in_off_right[i]  = (FF_CEIL_RSHIFT(s->in.row_right,  vsub) + s->in.off_rstep)  * iright->linesize[i] + FF_CEIL_RSHIFT(s->in.off_right  * s->pixstep[i], hsub);
        out_off_left[i]  = (FF_CEIL_RSHIFT(s->out.row_left,  vsub) + s->out.off_lstep) * oleft->linesize[i]  + FF_CEIL_RSHIFT(s->out.off_left  * s->pixstep[i], hsub);
        out_off_right[i] = (FF_CEIL_RSHIFT(s->out.row_right, vsub) + s->out.off_rstep) * oright->linesize[i] + FF_CEIL_RSHIFT(s->out.off_right * s->pixstep[i], hsub);
    }
    switch (s->out.format) {
    case ALTERNATING_LR:
    case ALTERNATING_RL:
    case SIDE_BY_SIDE_LR:
    case SIDE_BY_SIDE_RL:
    case SIDE_BY_SIDE_2_LR:
    case SIDE_BY_SIDE_2_RL:
    case ABOVE_BELOW_LR:
    case ABOVE_BELOW_RL:
    case ABOVE_BELOW_2_LR:
    case ABOVE_BELOW_2_RL:
    case INTERLEAVE_ROWS_LR:
    case INTERLEAVE_ROWS_RL:
        for (i = 0; i < s->nb_planes; i++) {
            av_image_copy_plane(oleft->data[i] + out_off_left[i],
                                oleft->linesize[i] * s->row_step,
                                ileft->data[i] + s->in_off_left[i],
                                ileft->linesize[i] * s->row_step,
                                s->linesize[i], s->pheight[i]);
            av_image_copy_plane(oright->data[i] + out_off_right[i],
                                oright->linesize[i] * s->row_step,
                                iright->data[i] + s->in_off_right[i],
                                iright->linesize[i] * s->row_step,
                                s->linesize[i], s->pheight[i]);
        }
        break;
    case MONO_L:
        iright = ileft;
    case MONO_R:
        for (i = 0; i < s->nb_planes; i++) {
            av_image_copy_plane(out->data[i], out->linesize[i],
                                iright->data[i] + s->in_off_left[i],
                                iright->linesize[i],
                                s->linesize[i], s->pheight[i]);
        }
        break;
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
        ThreadData td;
        td.ileft = ileft; td.iright = iright; td.out = out;
        ctx->internal->execute(ctx, filter_slice, &td, NULL,
                               FFMIN(s->out.height, ctx->graph->nb_threads));
        break;
    }
    default:
        av_assert0(0);
    }
    av_frame_free(&inpicref);
    av_frame_free(&s->prev);
    if (oright != oleft) {
        if (s->out.format == ALTERNATING_LR)
            FFSWAP(AVFrame *, oleft, oright);
        oright->pts = outlink->frame_count * s->ts_unit;
        ff_filter_frame(outlink, oright);
        out = oleft;
        oleft->pts = outlink->frame_count * s->ts_unit;
    } else if (s->in.format == ALTERNATING_LR ||
               s->in.format == ALTERNATING_RL) {
        out->pts = outlink->frame_count * s->ts_unit;
    }
    return ff_filter_frame(outlink, out);
}
