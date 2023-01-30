 *****************************************************************************/
static void Close( vlc_object_t *p_this )
{
    decoder_t     *p_dec = (decoder_t*) p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    var_Destroy( p_this, DVBSUB_CFG_PREFIX "x" );
    var_Destroy( p_this, DVBSUB_CFG_PREFIX "y" );
    var_Destroy( p_this, DVBSUB_CFG_PREFIX "position" );
    free_all( p_dec );
    free( p_sys );
}
