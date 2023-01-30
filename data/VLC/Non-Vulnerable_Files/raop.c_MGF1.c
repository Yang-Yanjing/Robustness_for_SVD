 */
static int MGF1( vlc_object_t *p_this,
                 unsigned char *mask, size_t l,
                 const unsigned char *Z, const size_t zLen,
                 const int Hash )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    gcry_error_t i_gcrypt_err;
    gcry_md_hd_t md_handle = NULL;
    unsigned int hLen;
    unsigned char *ps_md;
    uint32_t counter = 0;
    uint8_t C[4];
    size_t i_copylen;
    int i_err = VLC_SUCCESS;
    assert( mask != NULL );
    assert( Z != NULL );
    hLen = gcry_md_get_algo_dlen( Hash );
    i_gcrypt_err = gcry_md_open( &md_handle, Hash, 0 );
    if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    while ( l > 0 )
    {
        /* 3. For counter from 0 to \lceil{l / hLen}\rceil-1, do the following:
         * a. Convert counter to an octet string C of length 4 with the
         *    primitive I2OSP: C = I2OSP (counter, 4)
         */
        C[0] = (counter >> 24) & 0xff;
        C[1] = (counter >> 16) & 0xff;
        C[2] = (counter >> 8) & 0xff;
        C[3] = counter & 0xff;
        ++counter;
        /* b. Concatenate the hash of the seed Z and C to the octet string T:
         *    T = T || Hash (Z || C)
         */
        gcry_md_reset( md_handle );
        gcry_md_write( md_handle, Z, zLen );
        gcry_md_write( md_handle, C, 4 );
        ps_md = gcry_md_read( md_handle, Hash );
        /* 4. Output the leading l octets of T as the octet string mask. */
        i_copylen = __MIN( l, hLen );
        memcpy( mask, ps_md, i_copylen );
        mask += i_copylen;
        l -= i_copylen;
    }
error:
    gcry_md_close( md_handle );
    return i_err;
}
