}
block_t *aout_FiltersPlay (aout_filters_t *filters, block_t *block, int rate)
{
    int nominal_rate = 0;
    if (rate != INPUT_RATE_DEFAULT)
    {
        filter_t *rate_filter = filters->rate_filter;
        if (rate_filter == NULL)
            goto drop; /* Without linear, non-nominal rate is impossible. */
        /* Override input rate */
        nominal_rate = rate_filter->fmt_in.audio.i_rate;
        rate_filter->fmt_in.audio.i_rate =
            (nominal_rate * INPUT_RATE_DEFAULT) / rate;
    }
    block = aout_FiltersPipelinePlay (filters->tab, filters->count, block);
    if (filters->resampler != NULL)
    {   /* NOTE: the resampler needs to run even if resampling is 0.
         * The decoder and output rates can still be different. */
        filters->resampler->fmt_in.audio.i_rate += filters->resampling;
        block = aout_FiltersPipelinePlay (&filters->resampler, 1, block);
        filters->resampler->fmt_in.audio.i_rate -= filters->resampling;
    }
    if (nominal_rate != 0)
    {   /* Restore input rate */
        assert (filters->rate_filter != NULL);
        filters->rate_filter->fmt_in.audio.i_rate = nominal_rate;
    }
    return block;
drop:
    block_Release (block);
    return NULL;
}
