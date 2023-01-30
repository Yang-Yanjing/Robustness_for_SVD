 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    var_DelCallback( p_filter, CFG_PREFIX "intensity", FilterCallback, NULL );
    free( p_filter->p_sys );
}
