 *****************************************************************************/
static int bluraySetTitle(demux_t *p_demux, int i_title)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if (p_sys->b_menu) {
        if (i_title <= 0) {
            msg_Dbg(p_demux, "Playing TopMenu Title");
        } else if (i_title >= (int)p_sys->i_title - 1) {
            msg_Dbg(p_demux, "Playing FirstPlay Title");
            i_title = BLURAY_TITLE_FIRST_PLAY;
        } else {
            msg_Dbg(p_demux, "Playing Title %i", i_title);
        }
        if (bd_play_title(p_sys->bluray, i_title) == 0) {
            msg_Err(p_demux, "cannot play bd title '%d'", i_title);
            return VLC_EGENERIC;
        }
        return VLC_SUCCESS;
    }
    /* Looking for the main title, ie the longest duration */
    if (i_title < 0)
        i_title = p_sys->i_longest_title;
    else if ((unsigned)i_title > p_sys->i_title)
        return VLC_EGENERIC;
    msg_Dbg(p_demux, "Selecting Title %i", i_title);
    if (bd_select_title(p_sys->bluray, i_title) == 0) {
        msg_Err(p_demux, "cannot select bd title '%d'", i_title);
        return VLC_EGENERIC;
    }
    blurayResetParser(p_demux);
    return VLC_SUCCESS;
}
