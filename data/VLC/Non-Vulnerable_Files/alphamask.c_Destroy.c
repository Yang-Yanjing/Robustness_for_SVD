}
static void Destroy( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    filter_sys_t *p_sys = p_filter->p_sys;
    var_DelCallback( p_filter, CFG_PREFIX "mask", MaskCallback,
                     p_filter );
    vlc_mutex_destroy( &p_sys->mask_lock );
    if( p_sys->p_mask )
        picture_Release( p_sys->p_mask );
    free( p_sys );
}
