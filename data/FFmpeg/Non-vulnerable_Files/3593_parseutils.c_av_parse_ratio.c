int av_parse_ratio(AVRational *q, const char *str, int max,
                   int log_offset, void *log_ctx)
{
    char c;
    int ret;
    if (sscanf(str, "%d:%d%c", &q->num, &q->den, &c) != 2) {
        double d;
        ret = av_expr_parse_and_eval(&d, str, NULL, NULL,
                                     NULL, NULL, NULL, NULL,
                                     NULL, log_offset, log_ctx);
        if (ret < 0)
            return ret;
        *q = av_d2q(d, max);
    } else {
        av_reduce(&q->num, &q->den, q->num, q->den, max);
    }
    return 0;
}
