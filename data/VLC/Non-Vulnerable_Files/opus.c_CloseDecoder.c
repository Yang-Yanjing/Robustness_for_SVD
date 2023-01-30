 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t * p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( p_sys->p_st ) opus_multistream_decoder_destroy(p_sys->p_st);
    free( p_sys );
}
