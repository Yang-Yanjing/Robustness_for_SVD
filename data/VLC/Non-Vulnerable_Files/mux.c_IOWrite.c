 *****************************************************************************/
static int IOWrite( void *opaque, uint8_t *buf, int buf_size )
{
    sout_mux_t *p_mux = opaque;
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    int i_ret;
#ifdef AVFORMAT_DEBUG
    msg_Dbg( p_mux, "IOWrite %i bytes", buf_size );
#endif
    block_t *p_buf = block_Alloc( buf_size );
    if( buf_size > 0 ) memcpy( p_buf->p_buffer, buf, buf_size );
    if( p_sys->b_write_header )
        p_buf->i_flags |= BLOCK_FLAG_HEADER;
    if( p_sys->b_write_keyframe )
    {
        p_buf->i_flags |= BLOCK_FLAG_TYPE_I;
        p_sys->b_write_keyframe = false;
    }
    i_ret = sout_AccessOutWrite( p_mux->p_access, p_buf );
    return i_ret ? i_ret : -1;
}
