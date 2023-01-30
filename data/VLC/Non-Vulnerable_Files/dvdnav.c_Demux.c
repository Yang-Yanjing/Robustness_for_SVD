 *****************************************************************************/
static int Demux( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    uint8_t buffer[DVD_VIDEO_LB_LEN];
    uint8_t *packet = buffer;
    int i_event;
    int i_len;
    dvdnav_status_t status;
    if( p_sys->b_readahead )
        status = dvdnav_get_next_cache_block( p_sys->dvdnav, &packet, &i_event,
                                              &i_len );
    else
        status = dvdnav_get_next_block( p_sys->dvdnav, packet, &i_event,
                                        &i_len );
    if( status == DVDNAV_STATUS_ERR )
    {
        msg_Warn( p_demux, "cannot get next block (%s)",
                  dvdnav_err_to_string( p_sys->dvdnav ) );
        if( p_demux->info.i_title == 0 )
        {
            msg_Dbg( p_demux, "jumping to first title" );
            return ControlInternal( p_demux, DEMUX_SET_TITLE, 1 ) == VLC_SUCCESS ? 1 : -1;
        }
        return -1;
    }
    switch( i_event )
    {
    case DVDNAV_BLOCK_OK:   /* mpeg block */
        vlc_mutex_lock( &p_sys->still.lock );
        vlc_timer_schedule( p_sys->still.timer, false, 0, 0 );
        p_sys->still.b_enabled = false;
        vlc_mutex_unlock( &p_sys->still.lock );
        if( p_sys->b_reset_pcr )
        {
            es_out_Control( p_demux->out, ES_OUT_RESET_PCR );
            p_sys->b_reset_pcr = false;
        }
        DemuxBlock( p_demux, packet, i_len );
        if( p_sys->i_vobu_index > 0 )
        {
            if( p_sys->i_vobu_flush == p_sys->i_vobu_index )
                DemuxForceStill( p_demux );
            p_sys->i_vobu_index++;
        }
        break;
    case DVDNAV_NOP:    /* Nothing */
        msg_Dbg( p_demux, "DVDNAV_NOP" );
        break;
    case DVDNAV_STILL_FRAME:
    {
        dvdnav_still_event_t *event = (dvdnav_still_event_t*)packet;
        bool b_still_init = false;
        vlc_mutex_lock( &p_sys->still.lock );
        if( !p_sys->still.b_enabled )
        {
            msg_Dbg( p_demux, "DVDNAV_STILL_FRAME" );
            msg_Dbg( p_demux, "     - length=0x%x", event->length );
            p_sys->still.b_enabled = true;
            if( event->length != 0xff && p_sys->still.b_created )
            {
                mtime_t delay = event->length * CLOCK_FREQ;
                vlc_timer_schedule( p_sys->still.timer, false, delay, 0 );
            }
            b_still_init = true;
        }
        vlc_mutex_unlock( &p_sys->still.lock );
        if( b_still_init )
        {
            DemuxForceStill( p_demux );
            p_sys->b_reset_pcr = true;
        }
        msleep( 40000 );
        break;
    }
    case DVDNAV_SPU_CLUT_CHANGE:
    {
        int i;
        msg_Dbg( p_demux, "DVDNAV_SPU_CLUT_CHANGE" );
        /* Update color lookup table (16 *uint32_t in packet) */
        memcpy( p_sys->clut, packet, 16 * sizeof( uint32_t ) );
        /* HACK to get the SPU tracks registered in the right order */
        for( i = 0; i < 0x1f; i++ )
        {
            if( dvdnav_spu_stream_to_lang( p_sys->dvdnav, i ) != 0xffff )
                ESNew( p_demux, 0xbd20 + i );
        }
        /* END HACK */
        break;
    }
    case DVDNAV_SPU_STREAM_CHANGE:
    {
        dvdnav_spu_stream_change_event_t *event =
            (dvdnav_spu_stream_change_event_t*)packet;
        int i;
        msg_Dbg( p_demux, "DVDNAV_SPU_STREAM_CHANGE" );
        msg_Dbg( p_demux, "     - physical_wide=%d",
                 event->physical_wide );
        msg_Dbg( p_demux, "     - physical_letterbox=%d",
                 event->physical_letterbox);
        msg_Dbg( p_demux, "     - physical_pan_scan=%d",
                 event->physical_pan_scan );
        ESSubtitleUpdate( p_demux );
        p_sys->b_spu_change = true;
        /* HACK to get the SPU tracks registered in the right order */
        for( i = 0; i < 0x1f; i++ )
        {
            if( dvdnav_spu_stream_to_lang( p_sys->dvdnav, i ) != 0xffff )
                ESNew( p_demux, 0xbd20 + i );
        }
        /* END HACK */
        break;
    }
    case DVDNAV_AUDIO_STREAM_CHANGE:
    {
        dvdnav_audio_stream_change_event_t *event =
            (dvdnav_audio_stream_change_event_t*)packet;
        msg_Dbg( p_demux, "DVDNAV_AUDIO_STREAM_CHANGE" );
        msg_Dbg( p_demux, "     - physical=%d", event->physical );
        /* TODO */
        break;
    }
    case DVDNAV_VTS_CHANGE:
    {
        int32_t i_title = 0;
        int32_t i_part  = 0;
        int i;
        dvdnav_vts_change_event_t *event = (dvdnav_vts_change_event_t*)packet;
        msg_Dbg( p_demux, "DVDNAV_VTS_CHANGE" );
        msg_Dbg( p_demux, "     - vtsN=%d", event->new_vtsN );
        msg_Dbg( p_demux, "     - domain=%d", event->new_domain );
        /* reset PCR */
        es_out_Control( p_demux->out, ES_OUT_RESET_PCR );
        for( i = 0; i < PS_TK_COUNT; i++ )
        {
            ps_track_t *tk = &p_sys->tk[i];
            if( tk->b_seen )
            {
                es_format_Clean( &tk->fmt );
                if( tk->es ) es_out_Del( p_demux->out, tk->es );
            }
            tk->b_seen = false;
        }
        uint32_t i_width, i_height;
        if( dvdnav_get_video_resolution( p_sys->dvdnav,
                                         &i_width, &i_height ) )
            i_width = i_height = 0;
        switch( dvdnav_get_video_aspect( p_sys->dvdnav ) )
        {
        case 0:
            p_sys->sar.i_num = 4 * i_height;
            p_sys->sar.i_den = 3 * i_width;
            break;
        case 3:
            p_sys->sar.i_num = 16 * i_height;
            p_sys->sar.i_den =  9 * i_width;
            break;
        default:
            p_sys->sar.i_num = 0;
            p_sys->sar.i_den = 0;
            break;
        }
        if( dvdnav_current_title_info( p_sys->dvdnav, &i_title,
                                       &i_part ) == DVDNAV_STATUS_OK )
        {
            if( i_title >= 0 && i_title < p_sys->i_title &&
                p_demux->info.i_title != i_title )
            {
                p_demux->info.i_update |= INPUT_UPDATE_TITLE;
                p_demux->info.i_title = i_title;
            }
        }
        break;
    }
    case DVDNAV_CELL_CHANGE:
    {
        int32_t i_title = 0;
        int32_t i_part  = 0;
        dvdnav_cell_change_event_t *event =
            (dvdnav_cell_change_event_t*)packet;
        msg_Dbg( p_demux, "DVDNAV_CELL_CHANGE" );
        msg_Dbg( p_demux, "     - cellN=%d", event->cellN );
        msg_Dbg( p_demux, "     - pgN=%d", event->pgN );
        msg_Dbg( p_demux, "     - cell_length=%"PRId64, event->cell_length );
        msg_Dbg( p_demux, "     - pg_length=%"PRId64, event->pg_length );
        msg_Dbg( p_demux, "     - pgc_length=%"PRId64, event->pgc_length );
        msg_Dbg( p_demux, "     - cell_start=%"PRId64, event->cell_start );
        msg_Dbg( p_demux, "     - pg_start=%"PRId64, event->pg_start );
        /* Store the lenght in time of the current PGC */
        p_sys->i_pgc_length = event->pgc_length / 90 * 1000;
        p_sys->i_vobu_index = 0;
        p_sys->i_vobu_flush = 0;
        /* FIXME is it correct or there is better way to know chapter change */
        if( dvdnav_current_title_info( p_sys->dvdnav, &i_title,
                                       &i_part ) == DVDNAV_STATUS_OK )
        {
            if( i_title >= 0 && i_title < p_sys->i_title )
            {
                p_demux->info.i_update |= INPUT_UPDATE_TITLE;
                p_demux->info.i_title = i_title;
                if( i_part >= 1 && i_part <= p_sys->title[i_title]->i_seekpoint )
                {
                    p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
                    p_demux->info.i_seekpoint = i_part - 1;
                }
            }
        }
        break;
    }
    case DVDNAV_NAV_PACKET:
    {
        p_sys->i_vobu_index = 1;
        p_sys->i_vobu_flush = 0;
        /* Look if we have need to force a flush (and when) */
        const pci_gi_t *p_pci_gi = &dvdnav_get_current_nav_pci( p_sys->dvdnav )->pci_gi;
        if( p_pci_gi->vobu_se_e_ptm != 0 && p_pci_gi->vobu_se_e_ptm < p_pci_gi->vobu_e_ptm )
        {
            const dsi_gi_t *p_dsi_gi = &dvdnav_get_current_nav_dsi( p_sys->dvdnav )->dsi_gi;
            if( p_dsi_gi->vobu_3rdref_ea != 0 )
                p_sys->i_vobu_flush = p_dsi_gi->vobu_3rdref_ea;
            else if( p_dsi_gi->vobu_2ndref_ea != 0 )
                p_sys->i_vobu_flush = p_dsi_gi->vobu_2ndref_ea;
            else if( p_dsi_gi->vobu_1stref_ea != 0 )
                p_sys->i_vobu_flush = p_dsi_gi->vobu_1stref_ea;
        }
#ifdef DVDNAV_DEBUG
        msg_Dbg( p_demux, "DVDNAV_NAV_PACKET" );
#endif
        /* A lot of thing to do here :
         *  - handle packet
         *  - fetch pts (for time display)
         *  - ...
         */
        DemuxBlock( p_demux, packet, i_len );
        if( p_sys->b_spu_change )
        {
            ButtonUpdate( p_demux, false );
            p_sys->b_spu_change = false;
        }
        break;
    }
    case DVDNAV_STOP:   /* EOF */
        msg_Dbg( p_demux, "DVDNAV_STOP" );
        if( p_sys->b_readahead )
            dvdnav_free_cache_block( p_sys->dvdnav, packet );
        return 0;
    case DVDNAV_HIGHLIGHT:
    {
        dvdnav_highlight_event_t *event = (dvdnav_highlight_event_t*)packet;
        msg_Dbg( p_demux, "DVDNAV_HIGHLIGHT" );
        msg_Dbg( p_demux, "     - display=%d", event->display );
        msg_Dbg( p_demux, "     - buttonN=%d", event->buttonN );
        ButtonUpdate( p_demux, false );
        break;
    }
    case DVDNAV_HOP_CHANNEL:
        msg_Dbg( p_demux, "DVDNAV_HOP_CHANNEL" );
        p_sys->i_vobu_index = 0;
        p_sys->i_vobu_flush = 0;
        es_out_Control( p_demux->out, ES_OUT_RESET_PCR );
        break;
    case DVDNAV_WAIT:
        msg_Dbg( p_demux, "DVDNAV_WAIT" );
        bool b_empty;
        es_out_Control( p_demux->out, ES_OUT_GET_EMPTY, &b_empty );
        if( !b_empty )
        {
            msleep( 40*1000 );
        }
        else
        {
            dvdnav_wait_skip( p_sys->dvdnav );
            p_sys->b_reset_pcr = true;
        }
        break;
    default:
        msg_Warn( p_demux, "Unknown event (0x%x)", i_event );
        break;
    }
    if( p_sys->b_readahead )
        dvdnav_free_cache_block( p_sys->dvdnav, packet );
    return 1;
}
