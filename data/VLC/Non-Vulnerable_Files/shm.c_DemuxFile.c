 */
static void DemuxFile (void *data)
{
    demux_t *demux = data;
    demux_sys_t *sys = demux->p_sys;
    /* Copy frame */
    block_t *block = block_File (sys->fd);
    if (block == NULL)
        return;
    block->i_pts = block->i_dts = mdate ();
    /* Send block */
    es_out_Control (demux->out, ES_OUT_SET_PCR, block->i_pts);
    es_out_Send (demux->out, sys->es, block);
}
