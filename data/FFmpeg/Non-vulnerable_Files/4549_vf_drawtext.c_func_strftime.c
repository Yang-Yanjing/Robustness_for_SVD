static int func_strftime(AVFilterContext *ctx, AVBPrint *bp,
                         char *fct, unsigned argc, char **argv, int tag)
{
    const char *fmt = argc ? argv[0] : "%Y-%m-%d %H:%M:%S";
    time_t now;
    struct tm tm;
    time(&now);
    if (tag == 'L')
        localtime_r(&now, &tm);
    else
        tm = *gmtime_r(&now, &tm);
    av_bprint_strftime(bp, fmt, &tm);
    return 0;
}
