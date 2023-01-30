 ************************************************************************/
static int CryptSetup( sout_access_out_t *p_access, char *key_file )
{
    sout_access_out_sys_t *p_sys = p_access->p_sys;
    uint8_t key[16];
    char *keyfile = NULL;
    if( !p_sys->key_uri ) /*No key uri, assume no encryption wanted*/
    {
        msg_Dbg( p_access, "No key uri, no encryption");
        return VLC_SUCCESS;
    }
    if( key_file )
        keyfile = strdup( key_file );
    else
        keyfile = var_InheritString( p_access, SOUT_CFG_PREFIX "key-file" );
    if( unlikely(keyfile == NULL) )
    {
        msg_Err( p_access, "No key-file, no encryption" );
        return VLC_EGENERIC;
    }
    vlc_gcrypt_init();
    /*Setup encryption cipher*/
    gcry_error_t err = gcry_cipher_open( &p_sys->aes_ctx, GCRY_CIPHER_AES,
                                         GCRY_CIPHER_MODE_CBC, 0 );
    if( err )
    {
        msg_Err( p_access, "Openin AES Cipher failed: %s", gpg_strerror(err));
        free( keyfile );
        return VLC_EGENERIC;
    }
    int keyfd = vlc_open( keyfile, O_RDONLY | O_NONBLOCK );
    if( unlikely( keyfd == -1 ) )
    {
        msg_Err( p_access, "Unable to open keyfile %s: %s", keyfile,
                 vlc_strerror_c(errno) );
        free( keyfile );
        gcry_cipher_close( p_sys->aes_ctx );
        return VLC_EGENERIC;
    }
    free( keyfile );
    ssize_t keylen = read( keyfd, key, 16 );
    close( keyfd );
    if( keylen < 16 )
    {
        msg_Err( p_access, "No key at least 16 octects (you provided %zd), no encryption", keylen );
        gcry_cipher_close( p_sys->aes_ctx );
        return VLC_EGENERIC;
    }
    err = gcry_cipher_setkey( p_sys->aes_ctx, key, 16 );
    if(err)
    {
        msg_Err(p_access, "Setting AES key failed: %s", gpg_strerror(err));
        gcry_cipher_close( p_sys->aes_ctx );
        return VLC_EGENERIC;
    }
    if( p_sys->b_generate_iv )
        vlc_rand_bytes( p_sys->aes_ivs, sizeof(uint8_t)*16);
    return VLC_SUCCESS;
}
