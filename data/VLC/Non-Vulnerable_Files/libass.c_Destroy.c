 *****************************************************************************/
static void Destroy( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    DecSysRelease( p_dec->p_sys );
}
