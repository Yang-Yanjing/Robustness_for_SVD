enum AVSampleFormat av_get_sample_fmt(const char *name)
{
    int i;
    for (i = 0; i < AV_SAMPLE_FMT_NB; i++)
        if (!strcmp(sample_fmt_info[i].name, name))
            return i;
    return AV_SAMPLE_FMT_NONE;
}
