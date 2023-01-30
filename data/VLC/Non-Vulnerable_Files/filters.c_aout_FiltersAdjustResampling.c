}
bool aout_FiltersAdjustResampling (aout_filters_t *filters, int adjust)
{
    if (filters->resampler == NULL)
        return false;
    if (adjust)
        filters->resampling += adjust;
    else
        filters->resampling = 0;
    return filters->resampling != 0;
}
