}
static void blurayStillImage( demux_t *p_demux, unsigned i_timeout )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    /* time period elapsed ? */
    if (p_sys->i_still_end_time > 0 && p_sys->i_still_end_time <= mdate()) {
        msg_Dbg(p_demux, "Still image end");
        bd_read_skip_still(p_sys->bluray);
        blurayResetStillImage(p_demux);
        return;
    }
    /* show last frame as still image */
    if (!p_sys->i_still_end_time) {
        if (i_timeout) {
            msg_Dbg(p_demux, "Still image (%d seconds)", i_timeout);
            p_sys->i_still_end_time = mdate() + i_timeout * CLOCK_FREQ;
        } else {
            msg_Dbg(p_demux, "Still image (infinite)");
            p_sys->i_still_end_time = -1;
        }
        /* flush demuxer and decoder (there won't be next video packet starting with ts PUSI) */
        streamFlush(p_sys);
        /* stop buffering */
        bool b_empty;
        es_out_Control( p_demux->out, ES_OUT_GET_EMPTY, &b_empty );
    }
    /* avoid busy loops (read returns no data) */
    msleep( 40000 );
}
