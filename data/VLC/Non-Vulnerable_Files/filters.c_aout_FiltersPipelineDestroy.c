 */
static void aout_FiltersPipelineDestroy(filter_t *const *filters, unsigned n)
{
    for( unsigned i = 0; i < n; i++ )
    {
        filter_t *p_filter = filters[i];
        module_unneed( p_filter, p_filter->p_module );
        vlc_object_release( p_filter );
    }
}
