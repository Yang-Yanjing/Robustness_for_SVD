}
static void blurayHandleEvent(demux_t *p_demux, const BD_EVENT *e)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    switch (e->event) {
    case BD_EVENT_TITLE:
        if (e->param == BLURAY_TITLE_FIRST_PLAY)
            p_demux->info.i_title = p_sys->i_title - 1;
        else
            p_demux->info.i_title = e->param;
        /* this is feature title, we don't know yet which playlist it will play (if any) */
        setTitleInfo(p_sys, NULL);
        /* reset title infos here ? */
        p_demux->info.i_update |= INPUT_UPDATE_TITLE | INPUT_UPDATE_SEEKPOINT; /* might be BD-J title with no video */
        break;
    case BD_EVENT_PLAYLIST:
        /* Start of playlist playback (?????.mpls) */
        blurayUpdatePlaylist(p_demux, e->param);
        break;
    case BD_EVENT_PLAYITEM:
        blurayUpdateCurrentClip(p_demux, e->param);
        break;
    case BD_EVENT_CHAPTER:
        if (e->param && e->param < 0xffff)
          p_demux->info.i_seekpoint = e->param - 1;
        else
          p_demux->info.i_seekpoint = 0;
        p_demux->info.i_update |= INPUT_UPDATE_SEEKPOINT;
        break;
    case BD_EVENT_ANGLE:
        break;
    case BD_EVENT_MENU:
        p_sys->b_menu_open = e->param;
        break;
    case BD_EVENT_POPUP:
        p_sys->b_popup_available = e->param;
        /* TODO: show / hide pop-up menu button in gui ? */
        break;
    /*
     * stream selection events
     */
    case BD_EVENT_AUDIO_STREAM:
    case BD_EVENT_PG_TEXTST_STREAM:
        blurayStreamSelect(p_demux, e->event, e->param);
        break;
    case BD_EVENT_IG_STREAM:
        break;
    /*
     * playback control events
     */
    case BD_EVENT_STILL_TIME:
        blurayStillImage(p_demux, e->param);
        break;
    case BD_EVENT_DISCONTINUITY:
        /* reset demuxer (partially decoded PES packets must be dropped) */
        blurayResetParser(p_demux);
        break;
    case BD_EVENT_IDLE:
        /* nothing to do (ex. BD-J is preparing menus, waiting user input or running animation) */
        /* avoid busy loop (bd_read() returns no data) */
        msleep( 40000 );
        break;
    default:
        msg_Warn(p_demux, "event: %d param: %d", e->event, e->param);
        break;
    }
}
