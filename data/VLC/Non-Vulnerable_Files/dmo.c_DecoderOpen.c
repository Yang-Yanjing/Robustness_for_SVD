 *****************************************************************************/
static int DecoderOpen( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys;
    /* We can't open it now, because of ldt_keeper or something
     * Open/Decode/Close has to be done in the same thread */
    /* Probe if we support it */
    for( unsigned i = 0; decoders_table[i].i_fourcc != 0; i++ )
    {
        if( decoders_table[i].i_fourcc == p_dec->fmt_in.i_codec )
        {
            msg_Dbg( p_dec, "DMO codec for %4.4s may work with dll=%s",
                     (char*)&p_dec->fmt_in.i_codec,
                     decoders_table[i].psz_dll );
            goto found;
        }
    }
    return VLC_EGENERIC;
found:
    p_sys = p_dec->p_sys = malloc(sizeof(*p_sys));
    if( !p_sys )
        return VLC_ENOMEM;
    /* Set callbacks */
    p_dec->pf_decode_video = (picture_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    p_dec->pf_decode_audio = (block_t *(*)(decoder_t *, block_t **))
        DecodeBlock;
    vlc_mutex_init( &p_sys->lock );
    vlc_cond_init( &p_sys->wait_input );
    vlc_cond_init( &p_sys->wait_output );
    p_sys->b_works =
    p_sys->b_ready = false;
    p_sys->pp_input = NULL;
    TAB_INIT( p_sys->i_output, p_sys->pp_output );
    if( vlc_clone( &p_sys->thread, DecoderThread, p_dec,
                   VLC_THREAD_PRIORITY_INPUT ) )
        goto error;
    vlc_mutex_lock( &p_sys->lock );
    while( !p_sys->b_ready )
        vlc_cond_wait( &p_sys->wait_output, &p_sys->lock );
    vlc_mutex_unlock( &p_sys->lock );
    if( p_sys->b_works )
        return VLC_SUCCESS;
    vlc_join( p_sys->thread, NULL );
error:
    vlc_cond_destroy( &p_sys->wait_input );
    vlc_cond_destroy( &p_sys->wait_output );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
    return VLC_ENOMEM;
}
