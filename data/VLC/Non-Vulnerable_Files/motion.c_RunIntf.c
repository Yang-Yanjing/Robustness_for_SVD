#define HIGH_THRESHOLD 1000
static void *RunIntf( void *data )
{
    intf_thread_t *p_intf = data;
    int i_oldx = 0;
    for( ;; )
    {
        const char *psz_type;
        bool b_change = false;
        /* Wait a bit, get orientation, change filter if necessary */
#warning FIXME: check once (or less) per picture, not once per interval
        msleep( INTF_IDLE_SLEEP );
        int canc = vlc_savecancel();
        int i_x = motion_get_angle( p_intf->p_sys->p_motion );
        if( i_x < -HIGH_THRESHOLD && i_oldx > -LOW_THRESHOLD )
        {
            b_change = true;
            psz_type = "90";
        }
        else if( ( i_x > -LOW_THRESHOLD && i_oldx < -HIGH_THRESHOLD )
                 || ( i_x < LOW_THRESHOLD && i_oldx > HIGH_THRESHOLD ) )
        {
            b_change = true;
            psz_type = NULL;
        }
        else if( i_x > HIGH_THRESHOLD && i_oldx < LOW_THRESHOLD )
        {
            b_change = true;
            psz_type = "270";
        }
        if( b_change )
        {
#warning FIXME: refactor this plugin as a video filter!
            input_thread_t *p_input = pl_CurrentInput( p_intf );
            if( p_input )
            {
                vout_thread_t *p_vout;
                p_vout = input_GetVout( p_input );
                if( p_vout )
                {
                    if( psz_type != NULL )
                    {
                        var_Create( p_vout, "transform-type", VLC_VAR_STRING );
                        var_SetString( p_vout, "transform-type", psz_type );
                    }
                    else
                        var_Destroy( p_vout, "transform-type" );
                    var_SetString( p_vout, "video-filter",
                                   psz_type != NULL ? "transform" : "" );
                    vlc_object_release( p_vout );
                }
                vlc_object_release( p_input );
                i_oldx = i_x;
            }
        }
        vlc_restorecancel( canc );
    }
    assert(0);
}
