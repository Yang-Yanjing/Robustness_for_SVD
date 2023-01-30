 *****************************************************************************/
static int DvdReadSetArea( demux_t *p_demux, int i_title, int i_chapter,
                           int i_angle )
{
    VLC_UNUSED( i_angle );
    demux_sys_t *p_sys = p_demux->p_sys;
    int pgc_id = 0, pgn = 0;
    int i;
#define p_pgc p_sys->p_cur_pgc
#define p_vmg p_sys->p_vmg_file
#define p_vts p_sys->p_vts_file
    if( i_title >= 0 && i_title < p_sys->i_titles &&
        i_title != p_sys->i_title )
    {
        int i_start_cell, i_end_cell;
        if( p_sys->p_title != NULL )
        {
            DVDCloseFile( p_sys->p_title );
            p_sys->p_title = NULL;
        }
        if( p_vts != NULL ) ifoClose( p_vts );
        p_sys->i_title = i_title;
        /*
         *  We have to load all title information
         */
        msg_Dbg( p_demux, "open VTS %d, for title %d",
                 p_vmg->tt_srpt->title[i_title].title_set_nr, i_title + 1 );
        /* Ifo vts */
        if( !( p_vts = ifoOpen( p_sys->p_dvdread,
               p_vmg->tt_srpt->title[i_title].title_set_nr ) ) )
        {
            msg_Err( p_demux, "fatal error in vts ifo" );
            return VLC_EGENERIC;
        }
        /* Title position inside the selected vts */
        p_sys->i_ttn = p_vmg->tt_srpt->title[i_title].vts_ttn;
        /* Find title start/end */
        pgc_id = p_vts->vts_ptt_srpt->title[p_sys->i_ttn - 1].ptt[0].pgcn;
        pgn = p_vts->vts_ptt_srpt->title[p_sys->i_ttn - 1].ptt[0].pgn;
        p_pgc = p_vts->vts_pgcit->pgci_srp[pgc_id - 1].pgc;
        if( p_pgc->cell_playback == NULL )
        {
            msg_Err( p_demux, "Invalid PGC (cell_playback_offset)" );
            return VLC_EGENERIC;
        }
        p_sys->i_title_start_cell =
            i_start_cell = p_pgc->program_map[pgn - 1] - 1;
        p_sys->i_title_start_block =
            p_pgc->cell_playback[i_start_cell].first_sector;
        p_sys->i_title_end_cell =
            i_end_cell = p_pgc->nr_of_cells - 1;
        p_sys->i_title_end_block =
            p_pgc->cell_playback[i_end_cell].last_sector;
        p_sys->i_title_offset = 0;
        p_sys->i_title_blocks = 0;
        for( i = i_start_cell; i <= i_end_cell; i++ )
        {
            p_sys->i_title_blocks += p_pgc->cell_playback[i].last_sector -
                p_pgc->cell_playback[i].first_sector + 1;
        }
        msg_Dbg( p_demux, "title %d vts_title %d pgc %d pgn %d "
                 "start %d end %d blocks: %d",
                 i_title + 1, p_sys->i_ttn, pgc_id, pgn,
                 p_sys->i_title_start_block, p_sys->i_title_end_block,
                 p_sys->i_title_blocks );
        /*
         * Set properties for current chapter
         */
        p_sys->i_chapter = 0;
        p_sys->i_chapters =
            p_vts->vts_ptt_srpt->title[p_sys->i_ttn - 1].nr_of_ptts;
        pgc_id = p_vts->vts_ptt_srpt->title[
                    p_sys->i_ttn - 1].ptt[p_sys->i_chapter].pgcn;
        pgn = p_vts->vts_ptt_srpt->title[
                    p_sys->i_ttn - 1].ptt[p_sys->i_chapter].pgn;
        p_pgc = p_vts->vts_pgcit->pgci_srp[pgc_id - 1].pgc;
        p_sys->i_pack_len = 0;
        p_sys->i_next_cell =
            p_sys->i_cur_cell = p_pgc->program_map[pgn - 1] - 1;
        DvdReadFindCell( p_demux );
        p_sys->i_next_vobu = p_sys->i_cur_block =
            p_pgc->cell_playback[p_sys->i_cur_cell].first_sector;
        /*
         * Angle management
         */
        p_sys->i_angles = p_vmg->tt_srpt->title[i_title].nr_of_angles;
        if( p_sys->i_angle > p_sys->i_angles ) p_sys->i_angle = 1;
        /*
         * We've got enough info, time to open the title set data.
         */
        if( !( p_sys->p_title = DVDOpenFile( p_sys->p_dvdread,
            p_vmg->tt_srpt->title[i_title].title_set_nr,
            DVD_READ_TITLE_VOBS ) ) )
        {
            msg_Err( p_demux, "cannot open title (VTS_%02d_1.VOB)",
                     p_vmg->tt_srpt->title[i_title].title_set_nr );
            return VLC_EGENERIC;
        }
        //IfoPrintTitle( p_demux );
        /*
         * Destroy obsolete ES by reinitializing program 0
         * and find all ES in title with ifo data
         */
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
        if( p_demux->info.i_title != i_title )
        {
            p_demux->info.i_update |=
                INPUT_UPDATE_TITLE | INPUT_UPDATE_SEEKPOINT;
            p_demux->info.i_title = i_title;
            p_demux->info.i_seekpoint = 0;
        }
        /* TODO: re-add angles */
        ESNew( p_demux, 0xe0, 0 ); /* Video, FIXME ? */
        const video_attr_t *p_attr = &p_vts->vtsi_mat->vts_video_attr;
        int i_video_height = p_attr->video_format != 0 ? 576 : 480;
        int i_video_width;
        switch( p_attr->picture_size )
        {
        case 0:
            i_video_width = 720;
            break;
        case 1:
            i_video_width = 704;
            break;
        case 2:
            i_video_width = 352;
            break;
        default:
        case 3:
            i_video_width = 352;
            i_video_height /= 2;
            break;
        }
        switch( p_attr->display_aspect_ratio )
        {
        case 0:
            p_sys->i_sar_num = 4 * i_video_height;
            p_sys->i_sar_den = 3 * i_video_width;
            break;
        case 3:
            p_sys->i_sar_num = 16 * i_video_height;
            p_sys->i_sar_den =  9 * i_video_width;
            break;
        default:
            p_sys->i_sar_num = 0;
            p_sys->i_sar_den = 0;
            break;
        }
#define audio_control \
    p_sys->p_vts_file->vts_pgcit->pgci_srp[pgc_id-1].pgc->audio_control[i-1]
        /* Audio ES, in the order they appear in the .ifo */
        for( i = 1; i <= p_vts->vtsi_mat->nr_of_vts_audio_streams; i++ )
        {
            int i_position = 0;
            uint16_t i_id;
            //IfoPrintAudio( p_demux, i );
            /* Audio channel is active if first byte is 0x80 */
            if( audio_control & 0x8000 )
            {
                i_position = ( audio_control & 0x7F00 ) >> 8;
                msg_Dbg( p_demux, "audio position  %d", i_position );
                switch( p_vts->vtsi_mat->vts_audio_attr[i - 1].audio_format )
                {
                case 0x00: /* A52 */
                    i_id = (0x80 + i_position) | 0xbd00;
                    break;
                case 0x02:
                case 0x03: /* MPEG audio */
                    i_id = 0xc000 + i_position;
                    break;
                case 0x04: /* LPCM */
                    i_id = (0xa0 + i_position) | 0xbd00;
                    break;
                case 0x06: /* DTS */
                    i_id = (0x88 + i_position) | 0xbd00;
                    break;
                default:
                    i_id = 0;
                    msg_Err( p_demux, "unknown audio type %.2x",
                        p_vts->vtsi_mat->vts_audio_attr[i - 1].audio_format );
                }
                ESNew( p_demux, i_id, p_sys->p_vts_file->vtsi_mat->
                       vts_audio_attr[i - 1].lang_code );
            }
        }
#undef audio_control
#define spu_palette \
    p_sys->p_vts_file->vts_pgcit->pgci_srp[pgc_id-1].pgc->palette
        memcpy( p_sys->clut, spu_palette, 16 * sizeof( uint32_t ) );
#define spu_control \
    p_sys->p_vts_file->vts_pgcit->pgci_srp[pgc_id-1].pgc->subp_control[i-1]
        /* Sub Picture ES */
        for( i = 1; i <= p_vts->vtsi_mat->nr_of_vts_subp_streams; i++ )
        {
            int i_position = 0;
            uint16_t i_id;
            //IfoPrintSpu( p_sys, i );
            msg_Dbg( p_demux, "spu %d 0x%02x", i, spu_control );
            if( spu_control & 0x80000000 )
            {
                /*  there are several streams for one spu */
                if( p_vts->vtsi_mat->vts_video_attr.display_aspect_ratio )
                {
                    /* 16:9 */
                    switch( p_vts->vtsi_mat->vts_video_attr.permitted_df )
                    {
                    case 1: /* letterbox */
                        i_position = spu_control & 0xff;
                        break;
                    case 2: /* pan&scan */
                        i_position = ( spu_control >> 8 ) & 0xff;
                        break;
                    default: /* widescreen */
                        i_position = ( spu_control >> 16 ) & 0xff;
                        break;
                    }
                }
                else
                {
                    /* 4:3 */
                    i_position = ( spu_control >> 24 ) & 0x7F;
                }
                i_id = (0x20 + i_position) | 0xbd00;
                ESNew( p_demux, i_id, p_sys->p_vts_file->vtsi_mat->
                       vts_subp_attr[i - 1].lang_code );
            }
        }
#undef spu_control
    }
    else if( i_title != -1 && i_title != p_sys->i_title )
    {
        return VLC_EGENERIC; /* Couldn't set title */
    }
    /*
     * Chapter selection
     */
    if( i_chapter >= 0 && i_chapter < p_sys->i_chapters )
    {
        pgc_id = p_vts->vts_ptt_srpt->title[
                     p_sys->i_ttn - 1].ptt[i_chapter].pgcn;
        pgn = p_vts->vts_ptt_srpt->title[
                  p_sys->i_ttn - 1].ptt[i_chapter].pgn;
        p_pgc = p_vts->vts_pgcit->pgci_srp[pgc_id - 1].pgc;
        if( p_pgc->cell_playback == NULL )
            return VLC_EGENERIC; /* Couldn't set chapter */
        p_sys->i_cur_cell = p_pgc->program_map[pgn - 1] - 1;
        p_sys->i_chapter = i_chapter;
        DvdReadFindCell( p_demux );
        p_sys->i_title_offset = 0;
        for( i = p_sys->i_title_start_cell; i < p_sys->i_cur_cell; i++ )
        {
            p_sys->i_title_offset += p_pgc->cell_playback[i].last_sector -
                p_pgc->cell_playback[i].first_sector + 1;
        }
        p_sys->i_pack_len = 0;
        p_sys->i_next_vobu = p_sys->i_cur_block =
            p_pgc->cell_playback[p_sys->i_cur_cell].first_sector;
        if( p_demux->info.i_seekpoint != i_chapter )
        {
            p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
            p_demux->info.i_seekpoint = i_chapter;
        }
    }
    else if( i_chapter != -1 )
    {
        return VLC_EGENERIC; /* Couldn't set chapter */
    }
#undef p_pgc
#undef p_vts
#undef p_vmg
    return VLC_SUCCESS;
}
