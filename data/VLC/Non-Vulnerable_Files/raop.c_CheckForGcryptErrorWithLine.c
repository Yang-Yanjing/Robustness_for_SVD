}
static int CheckForGcryptErrorWithLine( sout_stream_t *p_stream,
                                        gcry_error_t i_gcrypt_err,
                                        unsigned int i_line )
{
    if ( i_gcrypt_err != GPG_ERR_NO_ERROR )
    {
        msg_Err( p_stream, "gcrypt error (line %d): %s", i_line,
                 gpg_strerror( i_gcrypt_err ) );
        return 1;
    }
    return 0;
}
