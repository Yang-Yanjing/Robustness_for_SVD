}
static void blurayResetParser(demux_t *p_demux)
{
    /*
     * This is a hack and will have to be removed.
     * The parser should be flushed, and not destroy/created each time
     * we are changing title.
     */
    demux_sys_t *p_sys = p_demux->p_sys;
    if (p_sys->p_parser)
        stream_Delete(p_sys->p_parser);
    p_sys->p_parser = stream_DemuxNew(p_demux, "ts", p_sys->p_out);
    if (!p_sys->p_parser)
        msg_Err(p_demux, "Failed to create TS demuxer");
}
