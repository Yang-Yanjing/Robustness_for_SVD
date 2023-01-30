 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    var_DelCallback(p_filter,FILTER_PREFIX "window-size",
        AntiFlickerCallback, p_filter->p_sys);
    var_DelCallback(p_filter,FILTER_PREFIX "softening-size",
        AntiFlickerCallback, p_filter->p_sys);
    free(p_filter->p_sys->p_old_data);
    free( p_filter->p_sys );
}
