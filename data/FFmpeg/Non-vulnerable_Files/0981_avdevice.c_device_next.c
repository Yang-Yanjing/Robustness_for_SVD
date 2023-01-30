static void *device_next(void *prev, int output,
                         AVClassCategory c1, AVClassCategory c2)
{
    const AVClass *pc;
    AVClassCategory category = AV_CLASS_CATEGORY_NA;
    do {
        if (output) {
            if (!(prev = av_oformat_next(prev)))
                break;
            pc = ((AVOutputFormat *)prev)->priv_class;
        } else {
            if (!(prev = av_iformat_next(prev)))
                break;
            pc = ((AVInputFormat *)prev)->priv_class;
        }
        if (!pc)
            continue;
        category = pc->category;
    } while (category != c1 && category != c2);
    return prev;
}
