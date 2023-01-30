 *****************************************************************************/
static void DecoderClose( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    free( p_dec->p_sys );
}
