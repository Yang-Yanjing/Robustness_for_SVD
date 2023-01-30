 *****************************************************************************/
static void CloseDecoder( vlc_object_t *p_this )
{
    decoder_t *p_dec = (decoder_t *)p_this;
    decoder_sys_t *p_sys = p_dec->p_sys;
    if( BC_FUNC_PSYS(DtsFlushInput)( p_sys->bcm_handle, 2 ) != BC_STS_SUCCESS )
        goto error;
    if( BC_FUNC_PSYS(DtsStopDecoder)( p_sys->bcm_handle ) != BC_STS_SUCCESS )
        goto error;
    if( BC_FUNC_PSYS(DtsCloseDecoder)( p_sys->bcm_handle ) != BC_STS_SUCCESS )
        goto error;
    if( BC_FUNC_PSYS(DtsDeviceClose)( p_sys->bcm_handle ) != BC_STS_SUCCESS )
        goto error;
error:
    free( p_sys->p_sps_pps_buf );
#ifdef DEBUG_CRYSTALHD
    msg_Dbg( p_dec, "done cleaning up CrystalHD" );
#endif
    free( p_sys );
}
