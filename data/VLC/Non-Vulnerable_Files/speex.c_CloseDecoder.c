 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t * p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->p_state )
    {
        speex_decoder_destroy( p_sys->p_state );
        speex_bits_destroy( &p_sys->bits );
    }
    free( p_sys->p_header );
    free( p_sys );
}
