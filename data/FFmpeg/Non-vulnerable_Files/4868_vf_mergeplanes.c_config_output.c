static int config_output(AVFilterLink *outlink)
{
    AVFilterContext *ctx = outlink->src;
    MergePlanesContext *s = ctx->priv;
    InputParam inputsp[4];
    FFFrameSyncIn *in;
    int i, ret;
    if ((ret = ff_framesync_init(&s->fs, ctx, s->nb_inputs)) < 0)
        return ret;
    in = s->fs.in;
    s->fs.opaque = s;
    s->fs.on_event = process_frame;
    outlink->w = ctx->inputs[0]->w;
    outlink->h = ctx->inputs[0]->h;
    outlink->time_base = ctx->inputs[0]->time_base;
    outlink->frame_rate = ctx->inputs[0]->frame_rate;
    outlink->sample_aspect_ratio = ctx->inputs[0]->sample_aspect_ratio;
    s->planewidth[1]  =
    s->planewidth[2]  = FF_CEIL_RSHIFT(outlink->w, s->outdesc->log2_chroma_w);
    s->planewidth[0]  =
    s->planewidth[3]  = outlink->w;
    s->planeheight[1] =
    s->planeheight[2] = FF_CEIL_RSHIFT(outlink->h, s->outdesc->log2_chroma_h);
    s->planeheight[0] =
    s->planeheight[3] = outlink->h;
    for (i = 0; i < s->nb_inputs; i++) {
        InputParam *inputp = &inputsp[i];
        AVFilterLink *inlink = ctx->inputs[i];
        const AVPixFmtDescriptor *indesc = av_pix_fmt_desc_get(inlink->format);
        int j;
        if (outlink->sample_aspect_ratio.num != inlink->sample_aspect_ratio.num ||
            outlink->sample_aspect_ratio.den != inlink->sample_aspect_ratio.den) {
            av_log(ctx, AV_LOG_ERROR, "input #%d link %s SAR %d:%d "
                                      "does not match output link %s SAR %d:%d\n",
                                      i, ctx->input_pads[i].name,
                                      inlink->sample_aspect_ratio.num,
                                      inlink->sample_aspect_ratio.den,
                                      ctx->output_pads[0].name,
                                      outlink->sample_aspect_ratio.num,
                                      outlink->sample_aspect_ratio.den);
            return AVERROR(EINVAL);
        }
        inputp->planewidth[1]  =
        inputp->planewidth[2]  = FF_CEIL_RSHIFT(inlink->w, indesc->log2_chroma_w);
        inputp->planewidth[0]  =
        inputp->planewidth[3]  = inlink->w;
        inputp->planeheight[1] =
        inputp->planeheight[2] = FF_CEIL_RSHIFT(inlink->h, indesc->log2_chroma_h);
        inputp->planeheight[0] =
        inputp->planeheight[3] = inlink->h;
        inputp->nb_planes = av_pix_fmt_count_planes(inlink->format);
        for (j = 0; j < inputp->nb_planes; j++)
            inputp->depth[j] = indesc->comp[j].depth_minus1 + 1;
        in[i].time_base = inlink->time_base;
        in[i].sync   = 1;
        in[i].before = EXT_STOP;
        in[i].after  = EXT_STOP;
    }
    for (i = 0; i < s->nb_planes; i++) {
        const int input = s->map[i][1];
        const int plane = s->map[i][0];
        InputParam *inputp = &inputsp[input];
        if (plane + 1 > inputp->nb_planes) {
            av_log(ctx, AV_LOG_ERROR, "input %d does not have %d plane\n",
                                      input, plane);
            goto fail;
        }
        if (s->outdesc->comp[i].depth_minus1 + 1 != inputp->depth[plane]) {
            av_log(ctx, AV_LOG_ERROR, "output plane %d depth %d does not "
                                      "match input %d plane %d depth %d\n",
                                      i, s->outdesc->comp[i].depth_minus1 + 1,
                                      input, plane, inputp->depth[plane]);
            goto fail;
        }
        if (s->planewidth[i] != inputp->planewidth[plane]) {
            av_log(ctx, AV_LOG_ERROR, "output plane %d width %d does not "
                                      "match input %d plane %d width %d\n",
                                      i, s->planewidth[i],
                                      input, plane, inputp->planewidth[plane]);
            goto fail;
        }
        if (s->planeheight[i] != inputp->planeheight[plane]) {
            av_log(ctx, AV_LOG_ERROR, "output plane %d height %d does not "
                                      "match input %d plane %d height %d\n",
                                      i, s->planeheight[i],
                                      input, plane, inputp->planeheight[plane]);
            goto fail;
        }
    }
    return ff_framesync_configure(&s->fs);
fail:
    return AVERROR(EINVAL);
}
