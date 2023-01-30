 *****************************************************************************/
static void ButtonUpdate( demux_t *p_demux, bool b_mode )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int32_t i_title, i_part;
    dvdnav_current_title_info( p_sys->dvdnav, &i_title, &i_part );
    dvdnav_highlight_area_t hl;
    int32_t i_button;
    bool    b_button_ok;
    if( dvdnav_get_current_highlight( p_sys->dvdnav, &i_button )
        != DVDNAV_STATUS_OK )
    {
        msg_Err( p_demux, "dvdnav_get_current_highlight failed" );
        return;
    }
    b_button_ok = false;
    if( i_button > 0 && i_title ==  0 )
    {
        pci_t *pci = dvdnav_get_current_nav_pci( p_sys->dvdnav );
        b_button_ok = DVDNAV_STATUS_OK ==
                  dvdnav_get_highlight_area( pci, i_button, b_mode, &hl );
    }
    if( b_button_ok )
    {
        for( unsigned i = 0; i < 4; i++ )
        {
            uint32_t i_yuv = p_sys->clut[(hl.palette>>(16+i*4))&0x0f];
            uint8_t i_alpha = ( (hl.palette>>(i*4))&0x0f ) * 0xff / 0xf;
            p_sys->palette[i][0] = (i_yuv >> 16) & 0xff;
            p_sys->palette[i][1] = (i_yuv >> 0) & 0xff;
            p_sys->palette[i][2] = (i_yuv >> 8) & 0xff;
            p_sys->palette[i][3] = i_alpha;
        }
        vlc_global_lock( VLC_HIGHLIGHT_MUTEX );
        var_SetInteger( p_sys->p_input, "x-start", hl.sx );
        var_SetInteger( p_sys->p_input, "x-end",  hl.ex );
        var_SetInteger( p_sys->p_input, "y-start", hl.sy );
        var_SetInteger( p_sys->p_input, "y-end", hl.ey );
        var_SetAddress( p_sys->p_input, "menu-palette", p_sys->palette );
        var_SetBool( p_sys->p_input, "highlight", true );
        msg_Dbg( p_demux, "buttonUpdate %d", i_button );
    }
    else
    {
        msg_Dbg( p_demux, "buttonUpdate not done b=%d t=%d",
                 i_button, i_title );
        /* Show all */
        vlc_global_lock( VLC_HIGHLIGHT_MUTEX );
        var_SetBool( p_sys->p_input, "highlight", false );
    }
    vlc_global_unlock( VLC_HIGHLIGHT_MUTEX );
}
