}
static block_t *OggStreamFlush( sout_mux_t *p_mux,
                                ogg_stream_state *p_os, mtime_t i_pts )
{
    return OggStreamGetPage( p_mux, p_os, i_pts, true );
}
