}
static int EventMouse( vlc_object_t *p_vout, char const *psz_var,
                       vlc_value_t oldval, vlc_value_t val, void *p_data )
{
    demux_t *p_demux = p_data;
    demux_sys_t *p_sys = p_demux->p_sys;
    /* FIXME? PCI usage thread safe? */
    pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
    int x = val.coords.x;
    int y = val.coords.y;
    if( psz_var[6] == 'm' ) /* mouse-moved */
        dvdnav_mouse_select( p_sys->dvdnav, pci, x, y );
    else
    {
        assert( psz_var[6] == 'c' ); /* mouse-clicked */
        ButtonUpdate( p_demux, true );
        dvdnav_mouse_activate( p_sys->dvdnav, pci, x, y );
    }
    (void)p_vout;
    (void)oldval;
    return VLC_SUCCESS;
}
