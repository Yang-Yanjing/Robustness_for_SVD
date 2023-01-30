}
static int AudioChannel( vlc_object_t *obj, char const *cmd,
                         vlc_value_t old, vlc_value_t cur, void *dummy )
{
    intf_thread_t *p_intf = (intf_thread_t*)obj;
    vlc_object_t *p_aout = (vlc_object_t *)playlist_GetAout( pl_Get(p_intf) );
    if ( p_aout == NULL )
         return VLC_ENOOBJ;
    int ret = VLC_SUCCESS;
    if ( !*cur.psz_string )
    {
        /* Retrieve all registered ***. */
        vlc_value_t val, text;
        if ( var_Change( p_aout, "stereo-mode",
                         VLC_VAR_GETLIST, &val, &text ) < 0 )
        {
            ret = VLC_ENOVAR;
            goto out;
        }
        int i_value = var_GetInteger( p_aout, "stereo-mode" );
        msg_rc( "+----[ %s ]", cmd );
        for ( int i = 0; i < val.p_list->i_count; i++ )
        {
            if ( i_value == val.p_list->p_values[i].i_int )
                msg_rc( "| %"PRId64" - %s *", val.p_list->p_values[i].i_int,
                        text.p_list->p_values[i].psz_string );
            else
                msg_rc( "| %"PRId64" - %s", val.p_list->p_values[i].i_int,
                        text.p_list->p_values[i].psz_string );
        }
        var_FreeList( &val, &text );
        msg_rc( "+----[ end of %s ]", cmd );
    }
    else
        ret = var_SetInteger( p_aout, "stereo-mode", atoi( cur.psz_string ) );
out:
    vlc_object_release( p_aout );
    (void) old; (void) dummy;
    return ret;
}
