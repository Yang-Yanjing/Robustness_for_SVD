 *****************************************************************************/
static void PPChangeMode( filter_t *p_filter, const char *psz_name,
                          int i_quality )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    vlc_mutex_lock( &p_sys->lock );
    if( i_quality > 0 )
    {
        pp_mode *pp_mode = pp_get_mode_by_name_and_quality( psz_name ?
                                                              psz_name :
                                                              "default",
                                                              i_quality );
        if( pp_mode )
        {
            pp_free_mode( p_sys->pp_mode );
            p_sys->pp_mode = pp_mode;
        }
        else
            msg_Warn( p_filter, "Error while changing post processing mode. "
                      "Keeping previous mode." );
    }
    else
    {
        pp_free_mode( p_sys->pp_mode );
        p_sys->pp_mode = NULL;
    }
    vlc_mutex_unlock( &p_sys->lock );
}
