 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    demux_t     *p_demux = (demux_t*)p_this;
    demux_sys_t *p_sys = p_demux->p_sys;
    vlc_cancel( p_sys->thread );
    vlc_join( p_sys->thread, NULL );
    if ( p_sys->es )
        es_out_Del( p_demux->out, p_sys->es );
    freerdp_disconnect( p_sys->p_instance );
    freerdp_free( p_sys->p_instance );
#if FREERDP_VERSION_MAJOR == 1 && FREERDP_VERSION_MINOR < 2
    freerdp_channels_global_uninit();
#endif
    if ( p_sys->p_block )
        block_Release( p_sys->p_block );
    free( p_sys->psz_hostname );
    free( p_sys );
}
