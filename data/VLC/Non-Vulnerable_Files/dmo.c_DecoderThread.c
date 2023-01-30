}
static void *DecoderThread( void *data )
{
    decoder_t *p_dec = data;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( DecOpen( p_dec ) )
        return NULL; /* failed */
    vlc_mutex_lock( &p_sys->lock );
    for( ;; )
    {
        while( p_sys->b_ready && !p_sys->pp_input )
            vlc_cond_wait( &p_sys->wait_input, &p_sys->lock );
        if( !p_sys->b_ready )
            break;
        for( ;; )
        {
            void *p_output = DecBlock( p_dec, p_sys->pp_input );
            if( !p_output )
                break;
            TAB_APPEND( p_sys->i_output, p_sys->pp_output, p_output );
        }
        p_sys->pp_input = NULL;
        vlc_cond_signal( &p_sys->wait_output );
    }
    vlc_mutex_unlock( &p_sys->lock );
    DecClose( p_dec );
    return NULL;
}
