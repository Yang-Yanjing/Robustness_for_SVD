 *****************************************************************************/
static void CloseEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    speex_encoder_destroy( p_sys->p_state );
    speex_bits_destroy( &p_sys->bits );
    free( p_sys->p_buffer );
    free( p_sys );
}
