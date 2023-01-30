 *****************************************************************************/
static void DvdReadHandleDSI( demux_t *p_demux, uint8_t *p_data )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    navRead_DSI( &p_sys->dsi_pack, &p_data[DSI_START_BYTE] );
    /*
     * Determine where we go next.  These values are the ones we mostly
     * care about.
     */
    p_sys->i_cur_block = p_sys->dsi_pack.dsi_gi.nv_pck_lbn;
    p_sys->i_pack_len = p_sys->dsi_pack.dsi_gi.vobu_ea;
    /*
     * Store the timecodes so we can get the current time
     */
    p_sys->i_title_cur_time = (mtime_t) p_sys->dsi_pack.dsi_gi.nv_pck_scr / 90 * 1000;
    p_sys->i_cell_cur_time = (mtime_t) dvdtime_to_time( &p_sys->dsi_pack.dsi_gi.c_eltm, 0 );
    /*
     * If we're not at the end of this cell, we can determine the next
     * VOBU to display using the VOBU_SRI information section of the
     * DSI.  Using this value correctly follows the current angle,
     * avoiding the doubled scenes in The Matrix, and makes our life
     * really happy.
     */
    p_sys->i_next_vobu = p_sys->i_cur_block +
        ( p_sys->dsi_pack.vobu_sri.next_vobu & 0x7fffffff );
    if( p_sys->dsi_pack.vobu_sri.next_vobu != SRI_END_OF_CELL
        && p_sys->i_angle > 1 )
    {
        switch( ( p_sys->dsi_pack.sml_pbi.category & 0xf000 ) >> 12 )
        {
        case 0x4:
            /* Interleaved unit with no angle */
            if( p_sys->dsi_pack.sml_pbi.ilvu_sa != 0 )
            {
                p_sys->i_next_vobu = p_sys->i_cur_block +
                    p_sys->dsi_pack.sml_pbi.ilvu_sa;
                p_sys->i_pack_len = p_sys->dsi_pack.sml_pbi.ilvu_ea;
            }
            else
            {
                p_sys->i_next_vobu = p_sys->i_cur_block +
                    p_sys->dsi_pack.dsi_gi.vobu_ea + 1;
            }
            break;
        case 0x5:
            /* vobu is end of ilvu */
            if( p_sys->dsi_pack.sml_agli.data[p_sys->i_angle-1].address )
            {
                p_sys->i_next_vobu = p_sys->i_cur_block +
                    p_sys->dsi_pack.sml_agli.data[p_sys->i_angle-1].address;
                p_sys->i_pack_len = p_sys->dsi_pack.sml_pbi.ilvu_ea;
                break;
            }
        case 0x6:
            /* vobu is beginning of ilvu */
        case 0x9:
            /* next scr is 0 */
        case 0xa:
            /* entering interleaved section */
        case 0x8:
            /* non interleaved cells in interleaved section */
        default:
            p_sys->i_next_vobu = p_sys->i_cur_block +
                ( p_sys->dsi_pack.vobu_sri.next_vobu & 0x7fffffff );
            break;
        }
    }
    else if( p_sys->dsi_pack.vobu_sri.next_vobu == SRI_END_OF_CELL )
    {
        p_sys->i_cur_cell = p_sys->i_next_cell;
        /* End of title */
        if( p_sys->i_cur_cell >= p_sys->p_cur_pgc->nr_of_cells ) return;
        DvdReadFindCell( p_demux );
        p_sys->i_next_vobu =
            p_sys->p_cur_pgc->cell_playback[p_sys->i_cur_cell].first_sector;
        p_sys->i_cell_duration = (mtime_t)dvdtime_to_time( &p_sys->p_cur_pgc->cell_playback[p_sys->i_cur_cell].playback_time, 0 );
    }
#if 0
    msg_Dbg( p_demux, "scr %d lbn 0x%02x vobu_ea %d vob_id %d c_id %d c_time %lld",
             p_sys->dsi_pack.dsi_gi.nv_pck_scr,
             p_sys->dsi_pack.dsi_gi.nv_pck_lbn,
             p_sys->dsi_pack.dsi_gi.vobu_ea,
             p_sys->dsi_pack.dsi_gi.vobu_vob_idn,
             p_sys->dsi_pack.dsi_gi.vobu_c_idn,
             dvdtime_to_time( &p_sys->dsi_pack.dsi_gi.c_eltm, 0 ) );
    msg_Dbg( p_demux, "cell duration: %lld",
             (mtime_t)dvdtime_to_time( &p_sys->p_cur_pgc->cell_playback[p_sys->i_cur_cell].playback_time, 0 ) );
    msg_Dbg( p_demux, "cat 0x%02x ilvu_ea %d ilvu_sa %d size %d",
             p_sys->dsi_pack.sml_pbi.category,
             p_sys->dsi_pack.sml_pbi.ilvu_ea,
             p_sys->dsi_pack.sml_pbi.ilvu_sa,
             p_sys->dsi_pack.sml_pbi.size );
    msg_Dbg( p_demux, "next_vobu %d next_ilvu1 %d next_ilvu2 %d",
             p_sys->dsi_pack.vobu_sri.next_vobu & 0x7fffffff,
             p_sys->dsi_pack.sml_agli.data[ p_sys->i_angle - 1 ].address,
             p_sys->dsi_pack.sml_agli.data[ p_sys->i_angle ].address);
#endif
}
