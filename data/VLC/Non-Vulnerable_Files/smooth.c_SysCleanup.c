}
static void SysCleanup( stream_sys_t *p_sys )
{
    if ( p_sys->sms_streams )
    {
        for ( int i=0; i< p_sys->sms_streams->i_count ; i++ )
            sms_Free( p_sys->sms_streams->pp_elems[i] );
        vlc_array_destroy( p_sys->sms_streams );
    }
    vlc_array_destroy( p_sys->selected_st );
    vlc_array_destroy( p_sys->download.chunks );
    if ( p_sys->init_chunks )
    {
        for ( int i=0; i< p_sys->init_chunks->i_count ; i++ )
            chunk_Free( p_sys->init_chunks->pp_elems[i] );
        vlc_array_destroy( p_sys->init_chunks );
    }
    sms_queue_free( p_sys->bws );
    free( p_sys->base_url );
}
