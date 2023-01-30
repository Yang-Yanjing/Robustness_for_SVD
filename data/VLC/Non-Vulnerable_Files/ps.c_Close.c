 *****************************************************************************/
static void Close( vlc_object_t * p_this )
{
    sout_mux_t      *p_mux = (sout_mux_t*)p_this;
    sout_mux_sys_t  *p_sys = p_mux->p_sys;
    block_t   *p_end;
    msg_Info( p_mux, "Close" );
    p_end = block_Alloc( 4 );
    p_end->p_buffer[0] = 0x00; p_end->p_buffer[1] = 0x00;
    p_end->p_buffer[2] = 0x01; p_end->p_buffer[3] = 0xb9;
    sout_AccessOutWrite( p_mux->p_access, p_end );
    free( p_sys );
}
