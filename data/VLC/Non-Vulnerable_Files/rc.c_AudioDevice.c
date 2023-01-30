}
static int AudioDevice( vlc_object_t *obj, char const *cmd,
                        vlc_value_t old, vlc_value_t cur, void *dummy )
{
    intf_thread_t *p_intf = (intf_thread_t *)obj;
    audio_output_t *p_aout = playlist_GetAout( pl_Get(p_intf) );
    if( p_aout == NULL )
        return VLC_ENOOBJ;
    if( !*cur.psz_string )
    {
        char **ids, **names;
        int n = aout_DevicesList( p_aout, &ids, &names );
        if( n < 0 )
            goto out;
        char *dev = aout_DeviceGet( p_aout );
        const char *devstr = (dev != NULL) ? dev : "";
        msg_rc( "+----[ %s ]", cmd );
        for ( int i = 0; i < n; i++ )
        {
            const char *fmt = "| %s - %s";
            if( !strcmp(devstr, ids[i]) )
                fmt = "| %s - %s *";
            msg_rc( fmt, ids[i], names[i] );
            free( names[i] );
            free( ids[i] );
        }
        msg_rc( "+----[ end of %s ]", cmd );
        free( dev );
        free( names );
        free( ids );
    }
    else
        aout_DeviceSet( p_aout, cur.psz_string );
out:
    vlc_object_release( p_aout );
    (void) old; (void) dummy;
    return VLC_SUCCESS;
}
