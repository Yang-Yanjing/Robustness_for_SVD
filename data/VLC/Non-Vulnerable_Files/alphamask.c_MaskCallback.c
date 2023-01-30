*****************************************************************************/
static int MaskCallback( vlc_object_t *p_this, char const *psz_var,
                         vlc_value_t oldval, vlc_value_t newval,
                         void *p_data )
{
    VLC_UNUSED(p_this); VLC_UNUSED(oldval);
    filter_t *p_filter = (filter_t *)p_data;
    filter_sys_t *p_sys = p_filter->p_sys;
    int i_ret = VLC_SUCCESS;
#define VAR_IS( a ) !strcmp( psz_var, CFG_PREFIX a )
    if( VAR_IS( "mask" ) )
    {
        vlc_mutex_lock( &p_sys->mask_lock );
        if( newval.psz_string && *newval.psz_string )
        {
            LoadMask( p_filter, newval.psz_string );
            if( !p_sys->p_mask )
            {
                msg_Err( p_filter, "Error while loading mask (%s).",
                         newval.psz_string );
                i_ret = VLC_EGENERIC;
            }
        }
        else if( p_sys->p_mask )
        {
            picture_Release( p_sys->p_mask );
            p_sys->p_mask = NULL;
        }
        vlc_mutex_unlock( &p_sys->mask_lock );
    }
#undef VAR_IS
    return i_ret;
}
