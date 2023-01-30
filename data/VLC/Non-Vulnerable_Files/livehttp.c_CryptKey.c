 ************************************************************************/
static int CryptKey( sout_access_out_t *p_access, uint32_t i_segment )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    if( !p_sys->b_generate_iv )
    {
        /* Use segment number as IV if randomIV isn't selected*/
        memset( p_sys->aes_ivs, 0, 16 * sizeof(uint8_t));
        p_sys->aes_ivs[15] = i_segment & 0xff;
        p_sys->aes_ivs[14] = (i_segment >> 8 ) & 0xff;
        p_sys->aes_ivs[13] = (i_segment >> 16 ) & 0xff;
        p_sys->aes_ivs[12] = (i_segment >> 24 ) & 0xff;
    }
    gcry_error_t err = gcry_cipher_setiv( p_sys->aes_ctx,
                                          p_sys->aes_ivs, 16);
    if( err )
    {
        msg_Err(p_access, "Setting AES IVs failed: %s", gpg_strerror(err) );
        gcry_cipher_close( p_sys->aes_ctx);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
