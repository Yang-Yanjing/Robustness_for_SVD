static int opt_abort_on(void *optctx, const char *opt, const char *arg)
{
    static const AVOption opts[] = {
        { "abort_on"        , NULL, 0, AV_OPT_TYPE_FLAGS, { .i64 = 0 }, INT64_MIN, INT64_MAX, .unit = "flags" },
        { "empty_output"    , NULL, 0, AV_OPT_TYPE_CONST, { .i64 = ABORT_ON_FLAG_EMPTY_OUTPUT     },    .unit = "flags" },
        { NULL },
    };
    static const AVClass class = {
        .class_name = "",
        .item_name  = av_default_item_name,
        .option     = opts,
        .version    = LIBAVUTIL_VERSION_INT,
    };
    const AVClass *pclass = &class;
    return av_opt_eval_flags(&pclass, &opts[0], arg, &abort_on_flags);
}
