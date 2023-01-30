}
static void DisplayVolume( intf_thread_t *p_intf, vout_thread_t *p_vout,
                           float vol )
{
    if( p_vout == NULL )
        return;
    ClearChannels( p_intf, p_vout );
    if( var_GetBool( p_vout, "fullscreen" ) )
        vout_OSDSlider( p_vout, p_intf->p_sys->slider_chan,
                        lroundf(vol * 100.f), OSD_VERT_SLIDER );
    DisplayMessage( p_vout, _( "Volume %ld%%" ), lroundf(vol * 100.f) );
}
