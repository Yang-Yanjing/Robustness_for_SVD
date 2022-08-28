static int filter_frame(AVFilterLink *inlink, AVFrame *in)
{
    AVFilterContext *ctx = inlink->dst;
    DrawGraphContext *s = ctx->priv;
    AVFilterLink *outlink = ctx->outputs[0];
    AVDictionary *metadata;
    AVDictionaryEntry *e;
    AVFrame *out = s->out;
    int i;
    if (!s->out || s->out->width  != outlink->w ||
                   s->out->height != outlink->h) {
        av_frame_free(&s->out);
        s->out = ff_get_video_buffer(outlink, outlink->w, outlink->h);
        out = s->out;
        if (!s->out) {
            av_frame_free(&in);
            return AVERROR(ENOMEM);
        }
        clear_image(s, out, outlink);
    }
    av_frame_copy_props(out, in);
    metadata = av_frame_get_metadata(in);
    for (i = 0; i < 4; i++) {
        double values[VAR_VARS_NB];
        int j, y, x, old;
        uint32_t fg, bg;
        float vf;
        e = av_dict_get(metadata, s->key[i], NULL, 0);
        if (!e || !e->value)
            continue;
        if (sscanf(e->value, "%f", &vf) != 1)
            continue;
        vf = av_clipf(vf, s->min, s->max);
        values[VAR_MIN] = s->min;
        values[VAR_MAX] = s->max;
        values[VAR_VAL] = vf;
        fg = av_expr_eval(s->fg_expr[i], values, NULL);
        bg = AV_RN32(s->bg);
        if (i == 0 && (s->x >= outlink->w || s->slide == 3)) {
            if (s->slide == 0 || s->slide == 1)
                s->x = 0;
            if (s->slide == 2) {
                s->x = outlink->w - 1;
                for (j = 0; j < outlink->h; j++) {
                    memmove(out->data[0] + j * out->linesize[0] ,
                            out->data[0] + j * out->linesize[0] + 4,
                            (outlink->w - 1) * 4);
                }
            } else if (s->slide == 3) {
                s->x = 0;
                for (j = 0; j < outlink->h; j++) {
                    memmove(out->data[0] + j * out->linesize[0] + 4,
                            out->data[0] + j * out->linesize[0],
                            (outlink->w - 1) * 4);
                }
            } else if (s->slide == 0) {
                clear_image(s, out, outlink);
            }
        }
        x = s->x;
        y = (outlink->h - 1) * (1 - ((vf - s->min) / (s->max - s->min)));
        switch (s->mode) {
        case 0:
            if (i == 0 && (s->slide > 0))
                for (j = 0; j < outlink->h; j++)
                    draw_dot(bg, x, j, out);
            old = AV_RN32(out->data[0] + y * out->linesize[0] + x * 4);
            for (j = y; j < outlink->h; j++) {
                if (old != bg &&
                    (AV_RN32(out->data[0] + j * out->linesize[0] + x * 4) != old) ||
                    AV_RN32(out->data[0] + FFMIN(j+1, outlink->h - 1) * out->linesize[0] + x * 4) != old) {
                    draw_dot(fg, x, j, out);
                    break;
                }
                draw_dot(fg, x, j, out);
            }
            break;
        case 1:
            if (i == 0 && (s->slide > 0))
                for (j = 0; j < outlink->h; j++)
                    draw_dot(bg, x, j, out);
            draw_dot(fg, x, y, out);
            break;
        case 2:
            if (s->first) {
                s->first = 0;
                s->prev_y[i] = y;
            }
            if (i == 0 && (s->slide > 0)) {
                for (j = 0; j < y; j++)
                    draw_dot(bg, x, j, out);
                for (j = outlink->h - 1; j > y; j--)
                    draw_dot(bg, x, j, out);
            }
            if (y <= s->prev_y[i]) {
                for (j = y; j <= s->prev_y[i]; j++)
                    draw_dot(fg, x, j, out);
            } else {
                for (j = s->prev_y[i]; j <= y; j++)
                    draw_dot(fg, x, j, out);
            }
            s->prev_y[i] = y;
            break;
        }
    }
    s->x++;
    av_frame_free(&in);
    return ff_filter_frame(outlink, av_frame_clone(s->out));
}
