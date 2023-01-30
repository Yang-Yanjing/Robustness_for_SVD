 *****************************************************************************/
static void DvdReadSeek( demux_t *p_demux, int i_block_offset )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    int i_chapter = 0;
    int i_cell = 0;
    int i_vobu = 0;
    int i_sub_cell = 0;
    int i_block;
#define p_pgc p_sys->p_cur_pgc
#define p_vts p_sys->p_vts_file
    /* Find cell */
    i_block = i_block_offset;
    for( i_cell = p_sys->i_title_start_cell;
         i_cell <= p_sys->i_title_end_cell; i_cell++ )
    {
        if( i_block < (int)p_pgc->cell_playback[i_cell].last_sector -
            (int)p_pgc->cell_playback[i_cell].first_sector + 1 ) break;
        i_block -= (p_pgc->cell_playback[i_cell].last_sector -
            p_pgc->cell_playback[i_cell].first_sector + 1);
    }
    if( i_cell > p_sys->i_title_end_cell )
    {
        msg_Err( p_demux, "couldn't find cell for block %i", i_block_offset );
        return;
    }
    i_block += p_pgc->cell_playback[i_cell].first_sector;
    p_sys->i_title_offset = i_block_offset;
    /* Find chapter */
    for( i_chapter = 0; i_chapter < p_sys->i_chapters; i_chapter++ )
    {
        int pgc_id, pgn, i_tmp;
        pgc_id = p_vts->vts_ptt_srpt->title[
                    p_sys->i_ttn - 1].ptt[i_chapter].pgcn;
        pgn = p_vts->vts_ptt_srpt->title[
                    p_sys->i_ttn - 1].ptt[i_chapter].pgn;
        i_tmp = p_vts->vts_pgcit->pgci_srp[pgc_id - 1].pgc->program_map[pgn-1];
        if( i_tmp > i_cell ) break;
    }
    if( i_chapter < p_sys->i_chapters &&
        p_demux->info.i_seekpoint != i_chapter )
    {
        p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
        p_demux->info.i_seekpoint = i_chapter;
    }
    /* Find vobu */
    while( (int)p_vts->vts_vobu_admap->vobu_start_sectors[i_vobu] <= i_block )
    {
        i_vobu++;
    }
    /* Find sub_cell */
    while( p_vts->vts_c_adt->cell_adr_table[i_sub_cell].start_sector <
           p_vts->vts_vobu_admap->vobu_start_sectors[i_vobu-1] )
    {
        i_sub_cell++;
    }
#if 1
    msg_Dbg( p_demux, "cell %d i_sub_cell %d chapter %d vobu %d "
             "cell_sector %d vobu_sector %d sub_cell_sector %d",
             i_cell, i_sub_cell, i_chapter, i_vobu,
             p_sys->p_cur_pgc->cell_playback[i_cell].first_sector,
             p_vts->vts_vobu_admap->vobu_start_sectors[i_vobu],
             p_vts->vts_c_adt->cell_adr_table[i_sub_cell - 1].start_sector);
#endif
    p_sys->i_cur_block = i_block;
    p_sys->i_next_vobu = p_vts->vts_vobu_admap->vobu_start_sectors[i_vobu];
    p_sys->i_pack_len = p_sys->i_next_vobu - i_block;
    p_sys->i_cur_cell = i_cell;
    p_sys->i_chapter = i_chapter;
    DvdReadFindCell( p_demux );
#undef p_vts
#undef p_pgc
    return;
}
