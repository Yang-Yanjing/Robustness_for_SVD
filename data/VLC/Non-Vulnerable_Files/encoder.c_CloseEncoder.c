 *****************************************************************************/
void CloseEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    /*FIXME: we should use avcodec_free_frame, but we don't require so new avcodec that has it*/
    av_freep( &p_sys->frame );
    vlc_avcodec_lock();
    avcodec_close( p_sys->p_context );
    vlc_avcodec_unlock();
    av_free( p_sys->p_context );
    av_free( p_sys->p_interleave_buf );
    av_free( p_sys->p_buffer );
    free( p_sys );
}
