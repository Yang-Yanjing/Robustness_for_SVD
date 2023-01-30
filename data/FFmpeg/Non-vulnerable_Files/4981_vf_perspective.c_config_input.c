static int config_input(AVFilterLink *inlink)
{
    double x0, x1, x2, x3, x4, x5, x6, x7, x8, q;
    double t0, t1, t2, t3;
    AVFilterContext *ctx = inlink->dst;
    PerspectiveContext *s = ctx->priv;
    double (*ref)[2] = s->ref;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(inlink->format);
    double values[VAR_VARS_NB] = { [VAR_W] = inlink->w, [VAR_H] = inlink->h };
    int h = inlink->h;
    int w = inlink->w;
    int x, y, i, j, ret;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            if (!s->expr_str[i][j])
                return AVERROR(EINVAL);
            ret = av_expr_parse_and_eval(&s->ref[i][j], s->expr_str[i][j],
                                         var_names, &values[0],
                                         NULL, NULL, NULL, NULL,
                                         0, 0, ctx);
            if (ret < 0)
                return ret;
        }
    }
    s->hsub = desc->log2_chroma_w;
    s->vsub = desc->log2_chroma_h;
    s->nb_planes = av_pix_fmt_count_planes(inlink->format);
    if ((ret = av_image_fill_linesizes(s->linesize, inlink->format, inlink->w)) < 0)
        return ret;
    s->height[1] = s->height[2] = FF_CEIL_RSHIFT(inlink->h, desc->log2_chroma_h);
    s->height[0] = s->height[3] = inlink->h;
    s->pv = av_realloc_f(s->pv, w * h, 2 * sizeof(*s->pv));
    if (!s->pv)
        return AVERROR(ENOMEM);
    switch (s->sense) {
    case PERSPECTIVE_SENSE_SOURCE:
        x6 = ((ref[0][0] - ref[1][0] - ref[2][0] + ref[3][0]) *
              (ref[2][1] - ref[3][1]) -
             ( ref[0][1] - ref[1][1] - ref[2][1] + ref[3][1]) *
              (ref[2][0] - ref[3][0])) * h;
        x7 = ((ref[0][1] - ref[1][1] - ref[2][1] + ref[3][1]) *
              (ref[1][0] - ref[3][0]) -
             ( ref[0][0] - ref[1][0] - ref[2][0] + ref[3][0]) *
              (ref[1][1] - ref[3][1])) * w;
        q =  ( ref[1][0] - ref[3][0]) * (ref[2][1] - ref[3][1]) -
             ( ref[2][0] - ref[3][0]) * (ref[1][1] - ref[3][1]);
        x0 = q * (ref[1][0] - ref[0][0]) * h + x6 * ref[1][0];
        x1 = q * (ref[2][0] - ref[0][0]) * w + x7 * ref[2][0];
        x2 = q *  ref[0][0] * w * h;
        x3 = q * (ref[1][1] - ref[0][1]) * h + x6 * ref[1][1];
        x4 = q * (ref[2][1] - ref[0][1]) * w + x7 * ref[2][1];
        x5 = q *  ref[0][1] * w * h;
        x8 = q * w * h;
        break;
    case PERSPECTIVE_SENSE_DESTINATION:
        t0 = ref[0][0] * (ref[3][1] - ref[1][1]) +
             ref[1][0] * (ref[0][1] - ref[3][1]) +
             ref[3][0] * (ref[1][1] - ref[0][1]);
        t1 = ref[1][0] * (ref[2][1] - ref[3][1]) +
             ref[2][0] * (ref[3][1] - ref[1][1]) +
             ref[3][0] * (ref[1][1] - ref[2][1]);
        t2 = ref[0][0] * (ref[3][1] - ref[2][1]) +
             ref[2][0] * (ref[0][1] - ref[3][1]) +
             ref[3][0] * (ref[2][1] - ref[0][1]);
        t3 = ref[0][0] * (ref[1][1] - ref[2][1]) +
             ref[1][0] * (ref[2][1] - ref[0][1]) +
             ref[2][0] * (ref[0][1] - ref[1][1]);
        x0 = t0 * t1 * w * (ref[2][1] - ref[0][1]);
        x1 = t0 * t1 * w * (ref[0][0] - ref[2][0]);
        x2 = t0 * t1 * w * (ref[0][1] * ref[2][0] - ref[0][0] * ref[2][1]);
        x3 = t1 * t2 * h * (ref[1][1] - ref[0][1]);
        x4 = t1 * t2 * h * (ref[0][0] - ref[1][0]);
        x5 = t1 * t2 * h * (ref[0][1] * ref[1][0] - ref[0][0] * ref[1][1]);
        x6 = t1 * t2 * (ref[1][1] - ref[0][1]) +
             t0 * t3 * (ref[2][1] - ref[3][1]);
        x7 = t1 * t2 * (ref[0][0] - ref[1][0]) +
             t0 * t3 * (ref[3][0] - ref[2][0]);
        x8 = t1 * t2 * (ref[0][1] * ref[1][0] - ref[0][0] * ref[1][1]) +
             t0 * t3 * (ref[2][0] * ref[3][1] - ref[2][1] * ref[3][0]);
        break;
    default:
        av_assert0(0);
    }
    for (y = 0; y < h; y++){
        for (x = 0; x < w; x++){
            int u, v;
            u = (int)floor(SUB_PIXELS * (x0 * x + x1 * y + x2) /
                                        (x6 * x + x7 * y + x8) + 0.5);
            v = (int)floor(SUB_PIXELS * (x3 * x + x4 * y + x5) /
                                        (x6 * x + x7 * y + x8) + 0.5);
            s->pv[x + y * w][0] = u;
            s->pv[x + y * w][1] = v;
        }
    }
    for (i = 0; i < SUB_PIXELS; i++){
        double d = i / (double)SUB_PIXELS;
        double temp[4];
        double sum = 0;
        for (j = 0; j < 4; j++)
            temp[j] = get_coeff(j - d - 1);
        for (j = 0; j < 4; j++)
            sum += temp[j];
        for (j = 0; j < 4; j++)
            s->coeff[i][j] = (int)floor((1 << COEFF_BITS) * temp[j] / sum + 0.5);
    }
    return 0;
}
