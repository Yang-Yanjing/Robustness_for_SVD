static void print_times(void)
{
    int i = 0;
    AVInputFormat *fmt = NULL;
    while ((fmt = av_iformat_next(fmt))) {
        if (fmt->flags & AVFMT_NOFILE)
            continue;
        if (time_array[i] > 1000000) {
            fprintf(stderr, "%12"PRIu64" cycles, %12s\n",
                    time_array[i], fmt->name);
        }
        i++;
    }
}
