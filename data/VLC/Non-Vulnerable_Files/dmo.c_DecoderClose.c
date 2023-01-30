 *****************************************************************************/
static void DecoderClose( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t*)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    vlc_mutex_lock( &p_sys->lock );
    p_sys->b_ready = false;
    vlc_cond_signal( &p_sys->wait_input );
    vlc_mutex_unlock( &p_sys->lock );
    vlc_join( p_sys->thread, NULL );
    TAB_CLEAN( p_sys->i_output, p_sys->pp_output );
    vlc_cond_destroy( &p_sys->wait_input );
    vlc_cond_destroy( &p_sys->wait_output );
    vlc_mutex_destroy( &p_sys->lock );
    free( p_sys );
}
