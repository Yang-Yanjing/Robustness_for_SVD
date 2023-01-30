 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    FILE *stream = (void *)p_dec->p_sys;
    if( stream != NULL )
        fclose( stream );
}
