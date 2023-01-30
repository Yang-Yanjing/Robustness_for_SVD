 *****************************************************************************/
static int Demux (demux_t *demux)
{
    demux_sys_t *sys = demux->p_sys;
    /* MIDI Tick emulation (ping the decoder every 10ms) */
    if (sys->tick <= date_Get (&sys->pts))
    {
        block_t *tick = block_Alloc (1);
        if (unlikely(tick == NULL))
            return VLC_ENOMEM;
        tick->p_buffer[0] = 0xF9;
        tick->i_dts = tick->i_pts = sys->tick;
        es_out_Send (demux->out, sys->es, tick);
        es_out_Control (demux->out, ES_OUT_SET_PCR, sys->tick);
        sys->tick += TICK;
        return 1;
    }
    /* MIDI events in chronological order across all tracks */
    uint64_t pulse = sys->pulse;
    if (ReadEvents (demux, &pulse, demux->out))
        return VLC_EGENERIC;
    if (pulse == UINT64_MAX)
        return 0; /* all tracks are done */
    sys->pulse = pulse;
    return 1;
}
