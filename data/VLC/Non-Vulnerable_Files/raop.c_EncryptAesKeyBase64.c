}
static int EncryptAesKeyBase64( vlc_object_t *p_this, char **result )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    gcry_error_t i_gcrypt_err;
    gcry_sexp_t sexp_rsa_params = NULL;
    gcry_sexp_t sexp_input = NULL;
    gcry_sexp_t sexp_encrypted = NULL;
    gcry_sexp_t sexp_token_a = NULL;
    gcry_mpi_t mpi_pubkey = NULL;
    gcry_mpi_t mpi_exp = NULL;
    gcry_mpi_t mpi_input = NULL;
    gcry_mpi_t mpi_output = NULL;
    unsigned char ps_padded_key[256];
    unsigned char *ps_value;
    size_t i_value_size;
    int i_err;
    /* Add RSA-OAES-SHA1 padding */
    i_err = AddOaepPadding( p_this,
                            ps_padded_key, sizeof( ps_padded_key ),
                            p_sys->ps_aes_key, sizeof( p_sys->ps_aes_key ),
                            NULL, 0 );
    if ( i_err != VLC_SUCCESS )
        goto error;
    i_err = VLC_EGENERIC;
    /* Read public key */
    i_gcrypt_err = gcry_mpi_scan( &mpi_pubkey, GCRYMPI_FMT_USG,
                                  ps_raop_rsa_pubkey,
                                  sizeof( ps_raop_rsa_pubkey ) - 1, NULL );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
        goto error;
    /* Read exponent */
    i_gcrypt_err = gcry_mpi_scan( &mpi_exp, GCRYMPI_FMT_USG, ps_raop_rsa_exp,
                                  sizeof( ps_raop_rsa_exp ) - 1, NULL );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
        goto error;
    /* If the input data starts with a set bit (0x80), gcrypt thinks it's a
     * signed integer and complains. Prefixing it with a zero byte (\0)
     * works, but involves more work. Converting it to an MPI in our code is
     * cleaner.
     */
    i_gcrypt_err = gcry_mpi_scan( &mpi_input, GCRYMPI_FMT_USG,
                                  ps_padded_key, sizeof( ps_padded_key ),
                                  NULL);
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
        goto error;
    /* Build S-expression with RSA parameters */
    i_gcrypt_err = gcry_sexp_build( &sexp_rsa_params, NULL,
                                    "(public-key(rsa(n %m)(e %m)))",
                                    mpi_pubkey, mpi_exp );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
        goto error;
    /* Build S-expression for data */
    i_gcrypt_err = gcry_sexp_build( &sexp_input, NULL, "(data(value %m))",
                                    mpi_input );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
        goto error;
    /* Encrypt data */
    i_gcrypt_err = gcry_pk_encrypt( &sexp_encrypted, sexp_input,
                                    sexp_rsa_params );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
        goto error;
    /* Extract encrypted data */
    sexp_token_a = gcry_sexp_find_token( sexp_encrypted, "a", 0 );
    if ( !sexp_token_a )
    {
        msg_Err( p_this , "Token 'a' not found in result S-expression" );
        goto error;
    }
    mpi_output = gcry_sexp_nth_mpi( sexp_token_a, 1, GCRYMPI_FMT_USG );
    if ( !mpi_output )
    {
        msg_Err( p_this, "Unable to extract MPI from result" );
        goto error;
    }
    /* Copy encrypted data into char array */
    i_gcrypt_err = gcry_mpi_aprint( GCRYMPI_FMT_USG, &ps_value, &i_value_size,
                                    mpi_output );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
    {
        goto error;
    }
    /* Encode in Base64 */
    *result = vlc_b64_encode_binary( ps_value, i_value_size );
    i_err = VLC_SUCCESS;
error:
    gcry_sexp_release( sexp_rsa_params );
    gcry_sexp_release( sexp_input );
    gcry_sexp_release( sexp_encrypted );
    gcry_sexp_release( sexp_token_a );
    gcry_mpi_release( mpi_pubkey );
    gcry_mpi_release( mpi_exp );
    gcry_mpi_release( mpi_input );
    gcry_mpi_release( mpi_output );
    return i_err;
}
