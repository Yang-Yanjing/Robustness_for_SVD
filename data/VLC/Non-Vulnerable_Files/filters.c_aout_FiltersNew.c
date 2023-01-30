 */
aout_filters_t *aout_FiltersNew (vlc_object_t *obj,
                                 const audio_sample_format_t *restrict infmt,
                                 const audio_sample_format_t *restrict outfmt,
                                 const aout_request_vout_t *request_vout)
{
    aout_filters_t *filters = malloc (sizeof (*filters));
    if (unlikely(filters == NULL))
        return NULL;
    filters->rate_filter = NULL;
    filters->resampler = NULL;
    filters->resampling = 0;
    filters->count = 0;
    /* Prepare format structure */
    aout_FormatPrint (obj, "input", infmt);
    audio_sample_format_t input_format = *infmt;
    audio_sample_format_t output_format = *outfmt;
    /* Callbacks (before reading values and also before return statement) */
    if (request_vout != NULL)
        var_AddCallback (obj, "visual", VisualizationCallback, NULL);
    /* Now add user filters */
    if (!AOUT_FMT_LINEAR(outfmt))
    {   /* Non-linear output: just convert formats, no filters/visu */
        if (!AOUT_FMTS_IDENTICAL(infmt, outfmt))
        {
            aout_FormatsPrint (obj, "pass-through:", infmt, outfmt);
            filters->tab[0] = FindConverter(obj, infmt, outfmt);
            if (filters->tab[0] == NULL)
            {
                msg_Err (obj, "cannot setup pass-through");
                goto error;
            }
            filters->count++;
        }
        return filters;
    }
    /* parse user filter lists */
    if (var_InheritBool (obj, "audio-time-stretch"))
    {
        if (AppendFilter(obj, "audio filter", "scaletempo",
                         filters, NULL, &input_format, &output_format) == 0)
            filters->rate_filter = filters->tab[filters->count - 1];
    }
    char *str = var_InheritString (obj, "audio-filter");
    if (str != NULL)
    {
        char *p = str, *name;
        while ((name = strsep (&p, " :")) != NULL)
        {
            AppendFilter(obj, "audio filter", name, filters,
                         NULL, &input_format, &output_format);
        }
        free (str);
    }
    if (request_vout != NULL)
    {
        char *visual = var_InheritString (obj, "audio-visual");
        if (visual != NULL && strcasecmp (visual, "none"))
            AppendFilter(obj, "visualization", visual, filters,
                         request_vout, &input_format, &output_format);
        free (visual);
    }
    /* convert to the output format (minus resampling) if necessary */
    output_format.i_rate = input_format.i_rate;
    if (aout_FiltersPipelineCreate (obj, filters->tab, &filters->count,
                              AOUT_MAX_FILTERS, &input_format, &output_format))
    {
        msg_Err (obj, "cannot setup filtering pipeline");
        goto error;
    }
    input_format = output_format;
    /* insert the resampler */
    output_format.i_rate = outfmt->i_rate;
    assert (AOUT_FMTS_IDENTICAL(&output_format, outfmt));
    filters->resampler = FindResampler (obj, &input_format,
                                        &output_format);
    if (filters->resampler == NULL && input_format.i_rate != outfmt->i_rate)
    {
        msg_Err (obj, "cannot setup a resampler");
        goto error;
    }
    if (filters->rate_filter == NULL)
        filters->rate_filter = filters->resampler;
    return filters;
error:
    aout_FiltersPipelineDestroy (filters->tab, filters->count);
    if (request_vout != NULL)
        var_DelCallback (obj, "visual", VisualizationCallback, NULL);
    free (filters);
    return NULL;
}
