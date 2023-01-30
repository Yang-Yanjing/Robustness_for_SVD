}
static int hls_DecodeSegmentData(stream_t *s, hls_stream_t *hls, segment_t *segment)
{
    /* Did the segment need to be decoded ? */
    if (segment->psz_key_path == NULL)
        return VLC_SUCCESS;
    /* Do we have loaded the key ? */
    if (!segment->b_key_loaded)
    {
        /* No ? try to download it now */
        if (hls_ManageSegmentKeys(s, hls) != VLC_SUCCESS)
            return VLC_EGENERIC;
    }
    /* For now, we only decode AES-128 data */
    gcry_error_t i_gcrypt_err;
    gcry_cipher_hd_t aes_ctx;
    /* Setup AES */
    i_gcrypt_err = gcry_cipher_open(&aes_ctx, GCRY_CIPHER_AES,
                                     GCRY_CIPHER_MODE_CBC, 0);
    if (i_gcrypt_err)
    {
        msg_Err(s, "gcry_cipher_open failed: %s", gpg_strerror(i_gcrypt_err));
        gcry_cipher_close(aes_ctx);
        return VLC_EGENERIC;
    }
    /* Set key */
    i_gcrypt_err = gcry_cipher_setkey(aes_ctx, segment->aes_key,
                                       sizeof(segment->aes_key));
    if (i_gcrypt_err)
    {
        msg_Err(s, "gcry_cipher_setkey failed: %s", gpg_strerror(i_gcrypt_err));
        gcry_cipher_close(aes_ctx);
        return VLC_EGENERIC;
    }
    if (hls->b_iv_loaded == false)
    {
        memset(hls->psz_AES_IV, 0, AES_BLOCK_SIZE);
        hls->psz_AES_IV[15] = segment->sequence & 0xff;
        hls->psz_AES_IV[14] = (segment->sequence >> 8)& 0xff;
        hls->psz_AES_IV[13] = (segment->sequence >> 16)& 0xff;
        hls->psz_AES_IV[12] = (segment->sequence >> 24)& 0xff;
    }
    i_gcrypt_err = gcry_cipher_setiv(aes_ctx, hls->psz_AES_IV,
                                      sizeof(hls->psz_AES_IV));
    if (i_gcrypt_err)
    {
        msg_Err(s, "gcry_cipher_setiv failed: %s", gpg_strerror(i_gcrypt_err));
        gcry_cipher_close(aes_ctx);
        return VLC_EGENERIC;
    }
    i_gcrypt_err = gcry_cipher_decrypt(aes_ctx,
                                       segment->data->p_buffer, /* out */
                                       segment->data->i_buffer,
                                       NULL, /* in */
                                       0);
    if (i_gcrypt_err)
    {
        msg_Err(s, "gcry_cipher_decrypt failed:  %s/%s\n", gcry_strsource(i_gcrypt_err), gcry_strerror(i_gcrypt_err));
        gcry_cipher_close(aes_ctx);
        return VLC_EGENERIC;
    }
    gcry_cipher_close(aes_ctx);
    /* remove the PKCS#7 padding from the buffer */
    int pad = segment->data->p_buffer[segment->data->i_buffer-1];
    if (pad <= 0 || pad > AES_BLOCK_SIZE)
    {
        msg_Err(s, "Bad padding character (0x%x), perhaps we failed to decrypt the segment with the correct key", pad);
        return VLC_EGENERIC;
    }
    int count = pad;
    while (count--)
    {
        if (segment->data->p_buffer[segment->data->i_buffer-1-count] != pad)
        {
                msg_Err(s, "Bad ending buffer, perhaps we failed to decrypt the segment with the correct key");
                return VLC_EGENERIC;
        }
    }
    /* not all the data is readable because of padding */
    segment->data->i_buffer -= pad;
    return VLC_SUCCESS;
}
