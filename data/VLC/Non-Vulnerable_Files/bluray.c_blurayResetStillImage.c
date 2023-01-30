}
static void blurayResetStillImage( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    if (p_sys->i_still_end_time) {
        p_sys->i_still_end_time = 0;
        blurayResetParser(p_demux);
        es_out_Control( p_demux->out, ES_OUT_RESET_PCR );
    }
}
