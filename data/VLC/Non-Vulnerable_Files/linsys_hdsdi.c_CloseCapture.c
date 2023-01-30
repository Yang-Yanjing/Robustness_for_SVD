}
static void CloseCapture( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    StopDecode( p_demux );
#ifdef HAVE_MMAP_SDIVIDEO
    for ( unsigned int i = 0; i < p_sys->i_vbuffers; i++ )
        munmap( p_sys->pp_vbuffers[i], p_sys->i_vbuffer_size );
    free( p_sys->pp_vbuffers );
#endif
    close( p_sys->i_vfd );
    if ( p_sys->i_max_channel != -1 )
    {
#ifdef HAVE_MMAP_SDIAUDIO
        for ( unsigned int i = 0; i < p_sys->i_abuffers; i++ )
            munmap( p_sys->pp_abuffers[i], p_sys->i_abuffer_size );
        free( p_sys->pp_abuffers );
#endif
        close( p_sys->i_afd );
    }
}
