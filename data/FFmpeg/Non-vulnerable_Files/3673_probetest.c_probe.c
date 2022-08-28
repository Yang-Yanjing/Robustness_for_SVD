static void probe(AVProbeData *pd, int type, int p, int size)
{
    int i = 0;
    AVInputFormat *fmt = NULL;
    while ((fmt = av_iformat_next(fmt))) {
        if (fmt->flags & AVFMT_NOFILE)
            continue;
        if (fmt->read_probe &&
            (!single_format || !strcmp(single_format, fmt->name))
        ) {
            int score;
            int64_t start = AV_READ_TIME();
            score = fmt->read_probe(pd);
            time_array[i] += AV_READ_TIME() - start;
            if (score > score_array[i] && score > AVPROBE_SCORE_MAX / 4) {
                score_array[i] = score;
                fprintf(stderr,
                        "Failure of %s probing code with score=%d type=%d p=%X size=%d\n",
                        fmt->name, score, type, p, size);
                failures++;
            }
        }
        i++;
    }
}
