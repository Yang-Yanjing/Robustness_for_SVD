}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    return p_stream->p_next->pf_del( p_stream->p_next, id );
}
