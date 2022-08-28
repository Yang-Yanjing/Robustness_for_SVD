static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    AVFilterLink *outlink = ctx->outputs[0];
    QPContext *s = ctx->priv;
    AVBufferRef *out_qp_table_buf;
    AVFrame *out = NULL;
    const int8_t *in_qp_table;
    int type, stride, ret;
    if (!s->qp_expr_str || ctx->is_disabled)
        return ff_filter_frame(outlink, in);
    out_qp_table_buf = av_buffer_alloc(s->h * s->qstride);
    if (!out_qp_table_buf) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    out = av_frame_clone(in);
    if (!out) {
        av_buffer_unref(&out_qp_table_buf);
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    in_qp_table = av_frame_get_qp_table(in, &stride, &type);
    av_frame_set_qp_table(out, out_qp_table_buf, s->qstride, type);
    if (s->evaluate_per_mb) {
        int y, x;
        for (y = 0; y < s->h; y++)
            for (x = 0; x < s->qstride; x++) {
                int qp = in_qp_table ? in_qp_table[x + stride * y] : NAN;
                double var_values[] = { !!in_qp_table, qp, x, y, s->qstride, s->h, 0};
                static const char *var_names[] = { "known", "qp", "x", "y", "w", "h", NULL };
                double temp_val;
                ret = av_expr_parse_and_eval(&temp_val, s->qp_expr_str,
                                            var_names, var_values,
                                            NULL, NULL, NULL, NULL, 0, 0, ctx);
                if (ret < 0)
                    goto fail;
                out_qp_table_buf->data[x + s->qstride * y] = lrintf(temp_val);
            }
    } else if (in_qp_table) {
        int y, x;
        for (y = 0; y < s->h; y++)
            for (x = 0; x < s->qstride; x++)
                out_qp_table_buf->data[x + s->qstride * y] = s->lut[129 +
                    ((int8_t)in_qp_table[x + stride * y])];
    } else {
        int y, x, qp = s->lut[0];
        for (y = 0; y < s->h; y++)
            for (x = 0; x < s->qstride; x++)
                out_qp_table_buf->data[x + s->qstride * y] = qp;
    }
    ret = ff_filter_frame(outlink, out);
    out = NULL;
fail:
    av_frame_free(&in);
    av_frame_free(&out);
    return ret;
}
