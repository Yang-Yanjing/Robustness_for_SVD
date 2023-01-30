static int parse_script(void *log, char *script, int script_len,
                            struct sbg_script *rscript)
{
    struct sbg_parser sp = {
        .log     = log,
        .script  = script,
        .end     = script + script_len,
        .cursor  = script,
        .line_no = 1,
        .err_msg = "",
        .scs = {
            
            .start_ts      = AV_NOPTS_VALUE,
            .sample_rate   = 44100,
            .opt_fade_time = 60 * AV_TIME_BASE,
        },
    };
    int r;
    lex_space(&sp);
    while (sp.cursor < sp.end) {
        r = parse_options(&sp);
        if (r < 0)
            goto fail;
        if (!r && !lex_line_end(&sp))
            break;
    }
    while (sp.cursor < sp.end) {
        r = parse_named_def(&sp);
        if (!r)
            r = parse_time_sequence(&sp, 0);
        if (!r)
            r = lex_line_end(&sp) ? 1 : AVERROR_INVALIDDATA;
        if (r < 0)
            goto fail;
    }
    *rscript = sp.scs;
    return 1;
fail:
    free_script(&sp.scs);
    if (!*sp.err_msg)
        if (r == AVERROR_INVALIDDATA)
            snprintf(sp.err_msg, sizeof(sp.err_msg), "syntax error");
    if (log && *sp.err_msg) {
        const char *ctx = sp.cursor;
        const char *ectx = av_x_if_null(memchr(ctx, '\n', sp.end - sp.cursor),
                                        sp.end);
        int lctx = ectx - ctx;
        const char *quote = "\"";
        if (lctx > 0 && ctx[lctx - 1] == '\r')
            lctx--;
        if (lctx == 0) {
            ctx = "the end of line";
            lctx = strlen(ctx);
            quote = "";
        }
        av_log(log, AV_LOG_ERROR, "Error line %d: %s near %s%.*s%s.\n",
               sp.line_no, sp.err_msg, quote, lctx, ctx, quote);
    }
    return r;
}
