 *****************************************************************************/
static void FreeSys( vlc_object_t *p_this, sout_stream_sys_t *p_sys )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    if ( p_sys->i_control_fd >= 0 )
        net_Close( p_sys->i_control_fd );
    if ( p_sys->i_stream_fd >= 0 )
        net_Close( p_sys->i_stream_fd );
    if ( p_sys->b_volume_callback )
        var_DelCallback( p_stream, SOUT_CFG_PREFIX "volume",
                         VolumeCallback, NULL );
    gcry_cipher_close( p_sys->aes_ctx );
    free( p_sys->p_sendbuf );
    free( p_sys->psz_host );
    free( p_sys->psz_password );
    free( p_sys->psz_url );
    free( p_sys->psz_session );
    free( p_sys->psz_client_instance );
    free( p_sys->psz_last_status_line );
    free( p_sys );
}
