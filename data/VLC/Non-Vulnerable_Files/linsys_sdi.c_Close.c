}
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t *)p_this;
    CloseCapture( p_demux );
    DemuxClose( p_this );
}
