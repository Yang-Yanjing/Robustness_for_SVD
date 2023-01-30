 */
void aout_FiltersDelete (vlc_object_t *obj, aout_filters_t *filters)
{
    if (filters->resampler != NULL)
        aout_FiltersPipelineDestroy (&filters->resampler, 1);
    aout_FiltersPipelineDestroy (filters->tab, filters->count);
    if (obj != NULL)
        var_DelCallback (obj, "visual", VisualizationCallback, NULL);
    free (filters);
}
