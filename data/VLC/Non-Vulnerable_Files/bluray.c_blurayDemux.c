#define NB_TS_PACKETS (200)
static int blurayDemux(demux_t *p_demux)
{
    demux_sys_t *p_sys = p_demux->p_sys;
    BD_EVENT e;
    block_t *p_block = block_Alloc(NB_TS_PACKETS * (int64_t)BD_TS_PACKET_SIZE);
    if (!p_block)
        return -1;
    int nread;
    if (p_sys->b_menu == false) {
        while (bd_get_event(p_sys->bluray, &e))
            blurayHandleEvent(p_demux, &e);
        nread = bd_read(p_sys->bluray, p_block->p_buffer,
                        NB_TS_PACKETS * BD_TS_PACKET_SIZE);
    } else {
        nread = bd_read_ext(p_sys->bluray, p_block->p_buffer,
                            NB_TS_PACKETS * BD_TS_PACKET_SIZE, &e);
        while (e.event != BD_EVENT_NONE) {
            blurayHandleEvent(p_demux, &e);
            bd_get_event(p_sys->bluray, &e);
        }
    }
    if (p_sys->current_overlay != -1) {
        bluray_overlay_t *ov = p_sys->p_overlays[p_sys->current_overlay];
        vlc_mutex_lock(&ov->lock);
        bool display = ov->status == ToDisplay;
        vlc_mutex_unlock(&ov->lock);
        if (display) {
            if (p_sys->p_vout == NULL)
                p_sys->p_vout = input_GetVout(p_sys->p_input);
            if (p_sys->p_vout != NULL) {
                var_AddCallback(p_sys->p_vout, "mouse-moved", onMouseEvent, p_demux);
                var_AddCallback(p_sys->p_vout, "mouse-clicked", onMouseEvent, p_demux);
                bluraySendOverlayToVout(p_demux);
            }
        }
    }
    if (nread <= 0) {
        block_Release(p_block);
        if (nread < 0)
            return -1;
        return 1;
    }
    p_block->i_buffer = nread;
    stream_DemuxSend(p_sys->p_parser, p_block);
    p_sys->b_flushed = false;
    return 1;
}
