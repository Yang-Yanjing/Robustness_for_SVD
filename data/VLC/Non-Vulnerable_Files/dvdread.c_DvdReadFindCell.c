 *****************************************************************************/
static void DvdReadFindCell( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    pgc_t *p_pgc;
    int   pgc_id, pgn;
    int   i = 0;
#define cell p_sys->p_cur_pgc->cell_playback
    if( cell[p_sys->i_cur_cell].block_type == BLOCK_TYPE_ANGLE_BLOCK )
    {
        p_sys->i_cur_cell += p_sys->i_angle - 1;
        while( cell[p_sys->i_cur_cell+i].block_mode != BLOCK_MODE_LAST_CELL )
        {
            i++;
        }
        p_sys->i_next_cell = p_sys->i_cur_cell + i + 1;
    }
    else
    {
        p_sys->i_next_cell = p_sys->i_cur_cell + 1;
    }
#undef cell
    if( p_sys->i_chapter + 1 >= p_sys->i_chapters ) return;
    pgc_id = p_sys->p_vts_file->vts_ptt_srpt->title[
                p_sys->i_ttn - 1].ptt[p_sys->i_chapter + 1].pgcn;
    pgn = p_sys->p_vts_file->vts_ptt_srpt->title[
              p_sys->i_ttn - 1].ptt[p_sys->i_chapter + 1].pgn;
    p_pgc = p_sys->p_vts_file->vts_pgcit->pgci_srp[pgc_id - 1].pgc;
    if( p_sys->i_cur_cell >= p_pgc->program_map[pgn - 1] - 1 )
    {
        p_sys->i_chapter++;
        if( p_sys->i_chapter < p_sys->i_chapters &&
            p_demux->info.i_seekpoint != p_sys->i_chapter )
        {
            p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
            p_demux->info.i_seekpoint = p_sys->i_chapter;
        }
    }
}
