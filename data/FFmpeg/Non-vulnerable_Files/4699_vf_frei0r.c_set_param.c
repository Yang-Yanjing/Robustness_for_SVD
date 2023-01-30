static int set_param(AVFilterContext *ctx, f0r_param_info_t info, int index, char *param)
{
    Frei0rContext *s = ctx->priv;
    union {
        double d;
        f0r_param_color_t col;
        f0r_param_position_t pos;
    } val;
    char *tail;
    uint8_t rgba[4];
    switch (info.type) {
    case F0R_PARAM_BOOL:
        if      (!strcmp(param, "y")) val.d = 1.0;
        else if (!strcmp(param, "n")) val.d = 0.0;
        else goto fail;
        break;
    case F0R_PARAM_DOUBLE:
        val.d = strtod(param, &tail);
        if (*tail || val.d == HUGE_VAL)
            goto fail;
        break;
    case F0R_PARAM_COLOR:
        if (sscanf(param, "%f/%f/%f", &val.col.r, &val.col.g, &val.col.b) != 3) {
            if (av_parse_color(rgba, param, -1, ctx) < 0)
                goto fail;
            val.col.r = rgba[0] / 255.0;
            val.col.g = rgba[1] / 255.0;
            val.col.b = rgba[2] / 255.0;
        }
        break;
    case F0R_PARAM_POSITION:
        if (sscanf(param, "%lf/%lf", &val.pos.x, &val.pos.y) != 2)
            goto fail;
        break;
    }
    s->set_param_value(s->instance, &val, index);
    return 0;
fail:
    av_log(ctx, AV_LOG_ERROR, "Invalid value '%s' for parameter '%s'.\n",
           param, info.name);
    return AVERROR(EINVAL);
}
