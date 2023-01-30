}
static void blurayUpdatePlaylist(demux_t *p_demux, unsigned i_playlist)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    blurayResetParser(p_demux);
    /* read title info and init some values */
    if (!p_sys->b_menu)
        p_demux->info.i_title = bd_get_current_title(p_sys->bluray);
    p_demux->info.i_seekpoint = 0;
    p_demux->info.i_update |= INPUT_UPDATE_TITLE | INPUT_UPDATE_SEEKPOINT;
    BLURAY_TITLE_INFO *p_title_info = bd_get_playlist_info(p_sys->bluray, i_playlist, 0);
    if (p_title_info) {
        blurayUpdateTitleInfo(p_sys->pp_title[p_demux->info.i_title], p_title_info);
    }
    setTitleInfo(p_sys, p_title_info);
    blurayResetStillImage(p_demux);
}
