}
static sout_stream_id_sys_t * Add( sout_stream_t *p_stream, es_format_t *p_fmt )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    bridge_t *p_bridge;
    bridged_es_t *p_es;
    char *psz_chain;
    int i;
    if( p_sys->b_inited || p_fmt->i_cat != VIDEO_ES )
        return NULL;
    /* Create decoder object */
    p_sys->p_decoder = vlc_object_create( p_stream, sizeof( decoder_t ) );
    if( !p_sys->p_decoder )
        return NULL;
    p_sys->p_decoder->p_module = NULL;
    p_sys->p_decoder->fmt_in = *p_fmt;
    p_sys->p_decoder->b_pace_control = false;
    p_sys->p_decoder->fmt_out = p_sys->p_decoder->fmt_in;
    p_sys->p_decoder->fmt_out.i_extra = 0;
    p_sys->p_decoder->fmt_out.p_extra = 0;
    p_sys->p_decoder->pf_decode_video = 0;
    p_sys->p_decoder->pf_vout_buffer_new = video_new_buffer_decoder;
    p_sys->p_decoder->pf_vout_buffer_del = video_del_buffer_decoder;
    p_sys->p_decoder->pf_picture_link    = video_link_picture_decoder;
    p_sys->p_decoder->pf_picture_unlink  = video_unlink_picture_decoder;
    p_sys->p_decoder->p_owner = malloc( sizeof(decoder_owner_sys_t) );
    if( !p_sys->p_decoder->p_owner )
    {
        vlc_object_release( p_sys->p_decoder );
        return NULL;
    }
    p_sys->p_decoder->p_owner->video = p_fmt->video;
    //p_sys->p_decoder->p_cfg = p_sys->p_video_cfg;
    p_sys->p_decoder->p_module =
        module_need( p_sys->p_decoder, "decoder", "$codec", false );
    if( !p_sys->p_decoder->p_module || !p_sys->p_decoder->pf_decode_video )
    {
        if( p_sys->p_decoder->p_module )
        {
            msg_Err( p_stream, "instanciated a non video decoder" );
            module_unneed( p_sys->p_decoder, p_sys->p_decoder->p_module );
        }
        else
        {
            msg_Err( p_stream, "cannot find decoder" );
        }
        free( p_sys->p_decoder->p_owner );
        vlc_object_release( p_sys->p_decoder );
        return NULL;
    }
    p_sys->b_inited = true;
    vlc_global_lock( VLC_MOSAIC_MUTEX );
    p_bridge = GetBridge( p_stream );
    if ( p_bridge == NULL )
    {
        vlc_object_t *p_libvlc = VLC_OBJECT( p_stream->p_libvlc );
        vlc_value_t val;
        p_bridge = xmalloc( sizeof( bridge_t ) );
        var_Create( p_libvlc, "mosaic-struct", VLC_VAR_ADDRESS );
        val.p_address = p_bridge;
        var_Set( p_libvlc, "mosaic-struct", val );
        p_bridge->i_es_num = 0;
        p_bridge->pp_es = NULL;
    }
    for ( i = 0; i < p_bridge->i_es_num; i++ )
    {
        if ( p_bridge->pp_es[i]->b_empty )
            break;
    }
    if ( i == p_bridge->i_es_num )
    {
        p_bridge->pp_es = xrealloc( p_bridge->pp_es,
                          (p_bridge->i_es_num + 1) * sizeof(bridged_es_t *) );
        p_bridge->i_es_num++;
        p_bridge->pp_es[i] = xmalloc( sizeof(bridged_es_t) );
    }
    p_sys->p_es = p_es = p_bridge->pp_es[i];
    p_es->i_alpha = var_GetInteger( p_stream, CFG_PREFIX "alpha" );
    p_es->i_x = var_GetInteger( p_stream, CFG_PREFIX "x" );
    p_es->i_y = var_GetInteger( p_stream, CFG_PREFIX "y" );
    //p_es->fmt = *p_fmt;
    p_es->psz_id = p_sys->psz_id;
    p_es->p_picture = NULL;
    p_es->pp_last = &p_es->p_picture;
    p_es->b_empty = false;
    vlc_global_unlock( VLC_MOSAIC_MUTEX );
    if ( p_sys->i_height || p_sys->i_width )
    {
        p_sys->p_image = image_HandlerCreate( p_stream );
    }
    else
    {
        p_sys->p_image = NULL;
    }
    msg_Dbg( p_stream, "mosaic bridge id=%s pos=%d", p_es->psz_id, i );
    /* Create user specified video filters */
    psz_chain = var_GetNonEmptyString( p_stream, CFG_PREFIX "vfilter" );
    msg_Dbg( p_stream, "psz_chain: %s", psz_chain );
    if( psz_chain )
    {
        p_sys->p_vf2 = filter_chain_New( p_stream, "video filter2", false,
                                         video_filter_buffer_allocation_init,
                                         NULL, p_sys->p_decoder->p_owner );
        es_format_t fmt;
        es_format_Copy( &fmt, &p_sys->p_decoder->fmt_out );
        if( p_sys->i_chroma )
            fmt.video.i_chroma = p_sys->i_chroma;
        filter_chain_Reset( p_sys->p_vf2, &fmt, &fmt );
        filter_chain_AppendFromString( p_sys->p_vf2, psz_chain );
        free( psz_chain );
    }
    else
    {
        p_sys->p_vf2 = NULL;
    }
    return (sout_stream_id_sys_t *)p_sys;
}
