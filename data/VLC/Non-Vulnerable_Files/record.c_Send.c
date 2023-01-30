}
static int Send( sout_stream_t *p_stream, sout_stream_id_sys_t *id,
                 block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    if( p_sys->i_date_start < 0 )
        p_sys->i_date_start = mdate();
    if( !p_sys->p_out &&
        ( mdate() - p_sys->i_date_start > p_sys->i_max_wait ||
          p_sys->i_size > p_sys->i_max_size ) )
    {
        msg_Dbg( p_stream, "Starting recording, waited %ds and %dbyte",
                 (int)((mdate() - p_sys->i_date_start)/1000000), (int)p_sys->i_size );
        OutputStart( p_stream );
    }
    OutputSend( p_stream, id, p_buffer );
    return VLC_SUCCESS;
}
