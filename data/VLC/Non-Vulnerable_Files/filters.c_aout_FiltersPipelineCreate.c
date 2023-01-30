 */
static int aout_FiltersPipelineCreate(vlc_object_t *obj, filter_t **filters,
                                      unsigned *count, unsigned max,
                                 const audio_sample_format_t *restrict infmt,
                                 const audio_sample_format_t *restrict outfmt)
{
    aout_FormatsPrint (obj, "conversion:", infmt, outfmt);
    max -= *count;
    filters += *count;
    /* There is a lot of second guessing on what the conversion plugins can
     * and cannot do. This seems hardly avoidable, the conversion problem need
     * to be reduced somehow. */
    audio_sample_format_t input = *infmt;
    unsigned n = 0;
    /* Encapsulate or decode non-linear formats */
    if (!AOUT_FMT_LINEAR(infmt) && infmt->i_format != outfmt->i_format)
    {
        if (n == max)
            goto overflow;
        filter_t *f = TryFormat (obj, VLC_CODEC_S32N, &input);
        if (f == NULL)
            f = TryFormat (obj, VLC_CODEC_FL32, &input);
        if (f == NULL)
        {
            msg_Err (obj, "cannot find %s for conversion pipeline",
                     "decoder");
            goto error;
        }
        filters[n++] = f;
    }
    assert (AOUT_FMT_LINEAR(&input));
    /* Remix channels */
    if (infmt->i_physical_channels != outfmt->i_physical_channels
     || infmt->i_original_channels != outfmt->i_original_channels)
    {   /* Remixing currently requires FL32... TODO: S16N */
        if (input.i_format != VLC_CODEC_FL32)
        {
            if (n == max)
                goto overflow;
            filter_t *f = TryFormat (obj, VLC_CODEC_FL32, &input);
            if (f == NULL)
            {
                msg_Err (obj, "cannot find %s for conversion pipeline",
                         "pre-mix converter");
                goto error;
            }
            filters[n++] = f;
        }
        if (n == max)
            goto overflow;
        audio_sample_format_t output;
        output.i_format = input.i_format;
        output.i_rate = input.i_rate;
        output.i_physical_channels = outfmt->i_physical_channels;
        output.i_original_channels = outfmt->i_original_channels;
        aout_FormatPrepare (&output);
        filter_t *f = FindConverter (obj, &input, &output);
        if (f == NULL)
        {
            msg_Err (obj, "cannot find %s for conversion pipeline",
                     "remixer");
            goto error;
        }
        input = output;
        filters[n++] = f;
    }
    /* Resample */
    if (input.i_rate != outfmt->i_rate)
    {   /* Resampling works with any linear format, but may be ugly. */
        if (n == max)
            goto overflow;
        audio_sample_format_t output = input;
        output.i_rate = outfmt->i_rate;
        filter_t *f = FindConverter (obj, &input, &output);
        if (f == NULL)
        {
            msg_Err (obj, "cannot find %s for conversion pipeline",
                     "resampler");
            goto error;
        }
        input = output;
        filters[n++] = f;
    }
    /* Format */
    if (input.i_format != outfmt->i_format)
    {
        if (max == 0)
            goto overflow;
        filter_t *f = TryFormat (obj, outfmt->i_format, &input);
        if (f == NULL)
        {
            msg_Err (obj, "cannot find %s for conversion pipeline",
                     "post-mix converter");
            goto error;
        }
        filters[n++] = f;
    }
    msg_Dbg (obj, "conversion pipeline complete");
    *count += n;
    return 0;
overflow:
    msg_Err (obj, "maximum of %u conversion filters reached", max);
    dialog_Fatal (obj, _("Audio filtering failed"),
                  _("The maximum number of filters (%u) was reached."), max);
error:
    aout_FiltersPipelineDestroy (filters, n);
    return -1;
}
