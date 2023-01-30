}
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    if ( p_fmt->i_id == p_sys->i_id )
    {
        msg_Dbg( p_stream, "delaying ID %d by %"PRId64,
                 p_sys->i_id, p_sys->i_delay );
        p_sys->id = p_stream->p_next->pf_add( p_stream->p_next, p_fmt );
        return p_sys->id;
    }
    return p_stream->p_next->pf_add( p_stream->p_next, p_fmt );
}
