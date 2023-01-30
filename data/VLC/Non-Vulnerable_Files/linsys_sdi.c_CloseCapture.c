}
static void CloseCapture( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    StopDecode( p_demux );
    for ( unsigned int i = 0; i < p_sys->i_buffers; i++ )
        munmap( p_sys->pp_buffers[i], p_sys->i_buffer_size );
    close( p_sys->i_fd );
    free( p_sys->pp_buffers );
}
