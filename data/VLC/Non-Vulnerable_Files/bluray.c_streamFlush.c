 *****************************************************************************/
static void streamFlush( demux_sys_t *p_sys )
{
    /*
     * MPEG-TS demuxer does not flush last video frame if size of PES packet is unknown.
     * Packet is flushed only when TS packet with PUSI flag set is received.
     *
     * Fix this by emitting (video) ts packet with PUSI flag set.
     * Add video sequence end code to payload so that also video decoder is flushed.
     * Set PES packet size in the payload so that it will be sent to decoder immediately.
     */
    if (p_sys->b_flushed)
        return;
    block_t *p_block = block_Alloc(192);
    if (!p_block)
        return;
    static const uint8_t seq_end_pes[] = {
        0x00, 0x00, 0x01, 0xe0, 0x00, 0x07, 0x80, 0x00, 0x00,  /* PES header */
        0x00, 0x00, 0x01, 0xb7,                                /* PES payload: sequence end */
    };
    static const uint8_t vid_pusi_ts[] = {
        0x00, 0x00, 0x00, 0x00,                /* TP extra header (ATC) */
        0x47, 0x50, 0x11, 0x30,                /* TP header */
        (192 - (4 + 5) - sizeof(seq_end_pes)), /* adaptation field length */
        0x80,                                  /* adaptation field: discontinuity indicator */
    };
    memset(p_block->p_buffer, 0, 192);
    memcpy(p_block->p_buffer, vid_pusi_ts, sizeof(vid_pusi_ts));
    memcpy(p_block->p_buffer + 192 - sizeof(seq_end_pes), seq_end_pes, sizeof(seq_end_pes));
    p_block->i_buffer = 192;
    /* set correct sequence end code */
    vlc_mutex_lock(&p_sys->pl_info_lock);
    if (p_sys->p_clip_info != NULL) {
        if (p_sys->p_clip_info->video_streams[0].coding_type > 2) {
            /* VC1 / H.264 sequence end */
            p_block->p_buffer[191] = 0x0a;
        }
    }
    vlc_mutex_unlock(&p_sys->pl_info_lock);
    stream_DemuxSend(p_sys->p_parser, p_block);
    p_sys->b_flushed = true;
}
