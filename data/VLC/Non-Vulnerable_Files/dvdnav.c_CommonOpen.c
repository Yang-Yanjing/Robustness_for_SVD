 *****************************************************************************/
static int CommonOpen( vlc_object_t *p_this,
                       dvdnav_t *p_dvdnav, bool b_readahead )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys;
    int         i_angle;
    char        *psz_code;
    assert( p_dvdnav );
    /* Fill p_demux field */
    DEMUX_INIT_COMMON(); p_sys = p_demux->p_sys;
    p_sys->dvdnav = p_dvdnav;
    p_sys->b_reset_pcr = false;
    ps_track_init( p_sys->tk );
    p_sys->sar.i_num = 0;
    p_sys->sar.i_den = 0;
    p_sys->i_mux_rate = 0;
    p_sys->i_pgc_length = 0;
    p_sys->b_spu_change = false;
    p_sys->i_vobu_index = 0;
    p_sys->i_vobu_flush = 0;
    p_sys->b_readahead = b_readahead;
    if( 1 )
    {
        // Hack for libdvdnav CVS.
        // Without it dvdnav_get_number_of_titles() fails.
        // Remove when fixed in libdvdnav CVS.
        uint8_t buffer[DVD_VIDEO_LB_LEN];
        int i_event, i_len;
        if( dvdnav_get_next_block( p_sys->dvdnav, buffer, &i_event, &i_len )
              == DVDNAV_STATUS_ERR )
        {
            msg_Warn( p_demux, "dvdnav_get_next_block failed" );
        }
        dvdnav_sector_search( p_sys->dvdnav, 0, SEEK_SET );
    }
    /* Configure dvdnav */
    if( dvdnav_set_readahead_flag( p_sys->dvdnav, p_sys->b_readahead ) !=
          DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "cannot set read-a-head flag" );
    }
    if( dvdnav_set_PGC_positioning_flag( p_sys->dvdnav, 1 ) !=
          DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "cannot set PGC positioning flag" );
    }
    /* Set menu language */
    psz_code = DemuxGetLanguageCode( p_demux, "menu-language" );
    if( dvdnav_menu_language_select( p_sys->dvdnav, psz_code ) !=
        DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "can't set menu language to '%s' (%s)",
                  psz_code, dvdnav_err_to_string( p_sys->dvdnav ) );
        /* We try to fall back to 'en' */
        if( strcmp( psz_code, LANGUAGE_DEFAULT ) )
            dvdnav_menu_language_select( p_sys->dvdnav, (char*)LANGUAGE_DEFAULT );
    }
    free( psz_code );
    /* Set audio language */
    psz_code = DemuxGetLanguageCode( p_demux, "audio-language" );
    if( dvdnav_audio_language_select( p_sys->dvdnav, psz_code ) !=
        DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "can't set audio language to '%s' (%s)",
                  psz_code, dvdnav_err_to_string( p_sys->dvdnav ) );
        /* We try to fall back to 'en' */
        if( strcmp( psz_code, LANGUAGE_DEFAULT ) )
            dvdnav_audio_language_select( p_sys->dvdnav, (char*)LANGUAGE_DEFAULT );
    }
    free( psz_code );
    /* Set spu language */
    psz_code = DemuxGetLanguageCode( p_demux, "sub-language" );
    if( dvdnav_spu_language_select( p_sys->dvdnav, psz_code ) !=
        DVDNAV_STATUS_OK )
    {
        msg_Warn( p_demux, "can't set spu language to '%s' (%s)",
                  psz_code, dvdnav_err_to_string( p_sys->dvdnav ) );
        /* We try to fall back to 'en' */
        if( strcmp( psz_code, LANGUAGE_DEFAULT ) )
            dvdnav_spu_language_select(p_sys->dvdnav, (char*)LANGUAGE_DEFAULT );
    }
    free( psz_code );
    DemuxTitles( p_demux );
    if( var_CreateGetBool( p_demux, "dvdnav-menu" ) )
    {
        msg_Dbg( p_demux, "trying to go to dvd menu" );
        if( dvdnav_title_play( p_sys->dvdnav, 1 ) != DVDNAV_STATUS_OK )
        {
            msg_Err( p_demux, "cannot set title (can't decrypt DVD?)" );
            dialog_Fatal( p_demux, _("Playback failure"), "%s",
                            _("VLC cannot set the DVD's title. It possibly "
                              "cannot decrypt the entire disc.") );
            free( p_sys );
            return VLC_EGENERIC;
        }
        if( dvdnav_menu_call( p_sys->dvdnav, DVD_MENU_Title ) !=
            DVDNAV_STATUS_OK )
        {
            /* Try going to menu root */
            if( dvdnav_menu_call( p_sys->dvdnav, DVD_MENU_Root ) !=
                DVDNAV_STATUS_OK )
                    msg_Warn( p_demux, "cannot go to dvd menu" );
        }
    }
    i_angle = var_CreateGetInteger( p_demux, "dvdnav-angle" );
    if( i_angle <= 0 ) i_angle = 1;
    /* FIXME hack hack hack hack FIXME */
    /* Get p_input and create variable */
    p_sys->p_input = demux_GetParentInput( p_demux );
    var_Create( p_sys->p_input, "x-start", VLC_VAR_INTEGER );
    var_Create( p_sys->p_input, "y-start", VLC_VAR_INTEGER );
    var_Create( p_sys->p_input, "x-end", VLC_VAR_INTEGER );
    var_Create( p_sys->p_input, "y-end", VLC_VAR_INTEGER );
    var_Create( p_sys->p_input, "color", VLC_VAR_ADDRESS );
    var_Create( p_sys->p_input, "menu-palette", VLC_VAR_ADDRESS );
    var_Create( p_sys->p_input, "highlight", VLC_VAR_BOOL );
    /* catch vout creation event */
    var_AddCallback( p_sys->p_input, "intf-event", EventIntf, p_demux );
    p_sys->still.b_enabled = false;
    vlc_mutex_init( &p_sys->still.lock );
    if( !vlc_timer_create( &p_sys->still.timer, StillTimer, p_sys ) )
        p_sys->still.b_created = true;
    return VLC_SUCCESS;
}
