 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    access_t     *p_access = (access_t*)p_this;
    access_sys_t *p_sys = p_access->p_sys;
    if( p_sys->p_ev )
    {
        /* stop the event handler */
        vlc_cancel( p_sys->p_ev->thread );
        if( p_sys->p_raw1394 )
            raw1394_iso_shutdown( p_sys->p_raw1394 );
        vlc_join( p_sys->p_ev->thread, NULL );
        vlc_mutex_destroy( &p_sys->p_ev->lock );
        /* Cleanup frame data */
        if( p_sys->p_ev->p_frame )
        {
            block_ChainRelease( p_sys->p_ev->p_frame );
            p_sys->p_ev->p_frame = NULL;
            p_sys->p_ev->pp_last = &p_sys->p_frame;
        }
        free( p_sys->p_ev );
    }
    if( p_sys->p_frame )
        block_ChainRelease( p_sys->p_frame );
    if( p_sys->p_raw1394 )
        raw1394_destroy_handle( p_sys->p_raw1394 );
    AVCClose( p_access );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
}
