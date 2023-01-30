}
static int AppendFilter(vlc_object_t *obj, const char *type, const char *name,
                        aout_filters_t *restrict filters, const void *owner,
                        audio_sample_format_t *restrict infmt,
                        const audio_sample_format_t *restrict outfmt)
{
    const unsigned max = sizeof (filters->tab) / sizeof (filters->tab[0]);
    if (filters->count >= max)
    {
        msg_Err (obj, "maximum of %u filters reached", max);
        return -1;
    }
    filter_t *filter = CreateFilter (obj, type, name,
                                     (void *)owner, infmt, outfmt);
    if (filter == NULL)
    {
        msg_Err (obj, "cannot add user %s \"%s\" (skipped)", type, name);
        return -1;
    }
    /* convert to the filter input format if necessary */
    if (aout_FiltersPipelineCreate (obj, filters->tab, &filters->count,
                                    max - 1, infmt, &filter->fmt_in.audio))
    {
        msg_Err (filter, "cannot add user %s \"%s\" (skipped)", type, name);
        module_unneed (filter, filter->p_module);
        vlc_object_release (filter);
        return -1;
    }
    assert (filters->count < max);
    filters->tab[filters->count] = filter;
    filters->count++;
    *infmt = filter->fmt_out.audio;
    return 0;
}
}
static int AppendFilter(vlc_object_t *obj, const char *type, const char *name,
                        aout_filters_t *restrict filters, const void *owner,
                        audio_sample_format_t *restrict infmt,
                        const audio_sample_format_t *restrict outfmt)
{
    const unsigned max = sizeof (filters->tab) / sizeof (filters->tab[0]);
    if (filters->count >= max)
    {
        msg_Err (obj, "maximum of %u filters reached", max);
        return -1;
    }
