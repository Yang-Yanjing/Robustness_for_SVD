static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    AVFrame *out;
    RotContext *rot = ctx->priv;
    int angle_int, s, c, plane;
    double res;
    out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
    if (!out) {
        av_frame_free(&in);
        return AVERROR(ENOMEM);
    }
    av_frame_copy_props(out, in);
    rot->var_values[VAR_N] = inlink->frame_count;
    rot->var_values[VAR_T] = TS2T(in->pts, inlink->time_base);
    rot->angle = res = av_expr_eval(rot->angle_expr, rot->var_values, rot);
    av_log(ctx, AV_LOG_DEBUG, "n:%f time:%f angle:%f/PI\n",
           rot->var_values[VAR_N], rot->var_values[VAR_T], rot->angle/M_PI);
    angle_int = res * FIXP * 16;
    s = int_sin(angle_int);
    c = int_sin(angle_int + INT_PI/2);
    
    if (rot->fillcolor_enable)
        ff_fill_rectangle(&rot->draw, &rot->color, out->data, out->linesize,
                          0, 0, outlink->w, outlink->h);
    for (plane = 0; plane < rot->nb_planes; plane++) {
        int hsub = plane == 1 || plane == 2 ? rot->hsub : 0;
        int vsub = plane == 1 || plane == 2 ? rot->vsub : 0;
        const int outw = FF_CEIL_RSHIFT(outlink->w, hsub);
        const int outh = FF_CEIL_RSHIFT(outlink->h, vsub);
        ThreadData td = { .in = in,   .out  = out,
                          .inw  = FF_CEIL_RSHIFT(inlink->w, hsub),
                          .inh  = FF_CEIL_RSHIFT(inlink->h, vsub),
                          .outh = outh, .outw = outw,
                          .xi = -(outw-1) * c / 2, .yi =  (outw-1) * s / 2,
                          .xprime = -(outh-1) * s / 2,
                          .yprime = -(outh-1) * c / 2,
                          .plane = plane, .c = c, .s = s };
        ctx->internal->execute(ctx, filter_slice, &td, NULL, FFMIN(outh, ctx->graph->nb_threads));
    }
    av_frame_free(&in);
    return ff_filter_frame(outlink, out);
}
