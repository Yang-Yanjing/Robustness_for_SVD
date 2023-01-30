#define SAMPLES (RATE / 10)
static int Demux (demux_t *demux)
{
    demux_sys_t *sys = demux->p_sys;
    /* Next track */
    if (gme_track_ended (sys->emu))
    {
        msg_Dbg (demux, "track %u ended", sys->track_id);
        if (++sys->track_id >= (unsigned)gme_track_count (sys->emu))
            return 0;
        demux->info.i_update |= INPUT_UPDATE_TITLE;
        demux->info.i_title = sys->track_id;
        gme_start_track (sys->emu, sys->track_id);
    }
    block_t *block = block_Alloc (2 * 2 * SAMPLES);
    if (unlikely(block == NULL))
        return 0;
    gme_err_t ret = gme_play (sys->emu, 2 * SAMPLES, (void *)block->p_buffer);
    if (ret != NULL)
    {
        block_Release (block);
        msg_Err (demux, "%s", ret);
        return 0;
    }
    block->i_pts = block->i_dts = VLC_TS_0 + date_Get (&sys->pts);
    es_out_Control (demux->out, ES_OUT_SET_PCR, block->i_pts);
    es_out_Send (demux->out, sys->es, block);
    date_Increment (&sys->pts, SAMPLES);
    return 1;
}
