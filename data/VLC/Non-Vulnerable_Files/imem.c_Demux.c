 */
static int Demux(demux_t *demux)
{
    imem_sys_t *sys = (imem_sys_t*)demux->p_sys;
    if (sys->deadline == VLC_TS_INVALID)
        sys->deadline = sys->dts + 1;
    for (;;) {
        if (sys->deadline <= sys->dts)
            break;
        /* */
        int64_t dts, pts;
        unsigned flags;
        size_t buffer_size;
        void   *buffer;
        if (sys->source.get(sys->source.data, sys->source.cookie,
                            &dts, &pts, &flags, &buffer_size, &buffer))
            return 0;
        if (dts < 0)
            dts = pts;
        if (buffer_size > 0) {
            block_t *block = block_Alloc(buffer_size);
            if (block) {
                block->i_dts = dts >= 0 ? (1 + dts) : VLC_TS_INVALID;
                block->i_pts = pts >= 0 ? (1 + pts) : VLC_TS_INVALID;
                memcpy(block->p_buffer, buffer, buffer_size);
                es_out_Control(demux->out, ES_OUT_SET_PCR, block->i_dts);
                es_out_Send(demux->out, sys->es, block);
            }
        }
        sys->dts = dts;
        sys->source.release(sys->source.data, sys->source.cookie,
                            buffer_size, buffer);
    }
    sys->deadline = VLC_TS_INVALID;
    return 1;
}
