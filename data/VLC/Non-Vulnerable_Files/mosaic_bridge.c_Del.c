}
static int Del( sout_stream_t *p_stream, sout_stream_id_sys_t *id )
{
    VLC_UNUSED(id);
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    bridge_t *p_bridge;
    bridged_es_t *p_es;
    bool b_last_es = true;
    int i;
    if( !p_sys->b_inited )
        return VLC_SUCCESS;
    if( p_sys->p_decoder != NULL )
    {
        decoder_owner_sys_t *p_owner = p_sys->p_decoder->p_owner;
        if( p_sys->p_decoder->p_module )
            module_unneed( p_sys->p_decoder, p_sys->p_decoder->p_module );
        if( p_sys->p_decoder->p_description )
            vlc_meta_Delete( p_sys->p_decoder->p_description );
        vlc_object_release( p_sys->p_decoder );
        free( p_owner );
    }
    /* Destroy user specified video filters */
    if( p_sys->p_vf2 )
        filter_chain_Delete( p_sys->p_vf2 );
    vlc_global_lock( VLC_MOSAIC_MUTEX );
    p_bridge = GetBridge( p_stream );
    p_es = p_sys->p_es;
    p_es->b_empty = true;
    while ( p_es->p_picture )
    {
        picture_t *p_next = p_es->p_picture->p_next;
        picture_Release( p_es->p_picture );
        p_es->p_picture = p_next;
    }
    for ( i = 0; i < p_bridge->i_es_num; i++ )
    {
        if ( !p_bridge->pp_es[i]->b_empty )
        {
            b_last_es = false;
            break;
        }
    }
    if ( b_last_es )
    {
        vlc_object_t *p_libvlc = VLC_OBJECT( p_stream->p_libvlc );
        for ( i = 0; i < p_bridge->i_es_num; i++ )
            free( p_bridge->pp_es[i] );
        free( p_bridge->pp_es );
        free( p_bridge );
        var_Destroy( p_libvlc, "mosaic-struct" );
    }
    vlc_global_unlock( VLC_MOSAIC_MUTEX );
    if ( p_sys->p_image )
    {
        image_HandlerDelete( p_sys->p_image );
    }
    p_sys->b_inited = false;
    return VLC_SUCCESS;
}
