AVFilterFormats *ff_planar_sample_fmts(void)
{
    AVFilterFormats *ret = NULL;
    int fmt;
    for (fmt = 0; av_get_bytes_per_sample(fmt)>0; fmt++)
        if (av_sample_fmt_is_planar(fmt))
            ff_add_format(&ret, fmt);
    return ret;
}
