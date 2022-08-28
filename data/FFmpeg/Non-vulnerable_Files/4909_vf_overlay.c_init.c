static av_cold int init(AVFilterContext *ctx)
{
    OverlayContext *s = ctx->priv;
    if (s->allow_packed_rgb) {
        av_log(ctx, AV_LOG_WARNING,
               "The rgb option is deprecated and is overriding the format option, use format instead\n");
        s->format = OVERLAY_FORMAT_RGB;
    }
    if (!s->dinput.repeatlast || s->eof_action == EOF_ACTION_PASS) {
        s->dinput.repeatlast = 0;
        s->eof_action = EOF_ACTION_PASS;
    }
    if (s->dinput.shortest || s->eof_action == EOF_ACTION_ENDALL) {
        s->dinput.shortest = 1;
        s->eof_action = EOF_ACTION_ENDALL;
    }
    s->dinput.process = do_blend;
    return 0;
}
