 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    DpbClean( p_dec );
    free( p_sys->p_gop_user_data );
    if( p_sys->p_synchro ) decoder_SynchroRelease( p_sys->p_synchro );
    if( p_sys->p_mpeg2dec ) mpeg2_close( p_sys->p_mpeg2dec );
    free( p_sys );
}
