}
static void DisplayPosition( intf_thread_t *p_intf, vout_thread_t *p_vout,
                             input_thread_t *p_input )
{
    char psz_duration[MSTRTIME_MAX_SIZE];
    char psz_time[MSTRTIME_MAX_SIZE];
    vlc_value_t time, pos;
    mtime_t i_seconds;
    if( p_vout == NULL ) return;
    ClearChannels( p_intf, p_vout );
    var_Get( p_input, "time", &time );
    i_seconds = time.i_time / 1000000;
    secstotimestr ( psz_time, i_seconds );
    var_Get( p_input, "length", &time );
    if( time.i_time > 0 )
    {
        secstotimestr( psz_duration, time.i_time / 1000000 );
        DisplayMessage( p_vout, "%s / %s", psz_time, psz_duration );
    }
    else if( i_seconds > 0 )
    {
        DisplayMessage( p_vout, "%s", psz_time );
    }
    if( var_GetBool( p_vout, "fullscreen" ) )
    {
        var_Get( p_input, "position", &pos );
        vout_OSDSlider( p_vout, p_intf->p_sys->slider_chan,
                        pos.f_float * 100, OSD_HOR_SLIDER );
    }
}
