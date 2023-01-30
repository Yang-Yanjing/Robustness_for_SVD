 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t*)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    module_unneed( &p_sys->volume, p_sys->module );
    vlc_object_release( &p_sys->volume );
}
