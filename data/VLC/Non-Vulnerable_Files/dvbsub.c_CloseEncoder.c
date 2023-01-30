 *****************************************************************************/
static void CloseEncoder( vlc_object_t *p_this )
{
    encoder_t *p_enc = (encoder_t *)p_this;
    encoder_sys_t *p_sys = p_enc->p_sys;
    var_Destroy( p_this , ENC_CFG_PREFIX "x" );
    var_Destroy( p_this , ENC_CFG_PREFIX "y" );
    var_Destroy( p_this , ENC_CFG_PREFIX "timeout" );
    if( p_sys->i_regions ) free( p_sys->p_regions );
    free( p_sys );
}
