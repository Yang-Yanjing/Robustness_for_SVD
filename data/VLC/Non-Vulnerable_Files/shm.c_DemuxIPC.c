#ifdef HAVE_SYS_SHM_H
static void DemuxIPC (void *data)
{
    demux_t *demux = data;
    demux_sys_t *sys = demux->p_sys;
    /* Copy frame */
    block_t *block = block_Alloc (sys->mem.length);
    if (block == NULL)
        return;
    memcpy (block->p_buffer, sys->mem.addr, sys->mem.length);
    block->i_pts = block->i_dts = mdate ();
    /* Send block */
    es_out_Control (demux->out, ES_OUT_SET_PCR, block->i_pts);
    es_out_Send (demux->out, sys->es, block);
}
