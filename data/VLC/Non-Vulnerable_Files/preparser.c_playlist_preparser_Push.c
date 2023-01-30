}
void playlist_preparser_Push( playlist_preparser_t *p_preparser, input_item_t *p_item,
                              input_item_meta_request_option_t i_options )
{
    vlc_gc_incref( p_item );
    VLC_UNUSED( i_options );
    vlc_mutex_lock( &p_preparser->lock );
    INSERT_ELEM( p_preparser->pp_waiting, p_preparser->i_waiting,
                 p_preparser->i_waiting, p_item );
    if( !p_preparser->b_live )
    {
        if( vlc_clone_detach( NULL, Thread, p_preparser,
                              VLC_THREAD_PRIORITY_LOW ) )
            msg_Warn( p_preparser->object, "cannot spawn pre-parser thread" );
        else
            p_preparser->b_live = true;
    }
    vlc_mutex_unlock( &p_preparser->lock );
}
