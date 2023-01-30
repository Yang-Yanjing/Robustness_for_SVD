}
static void SendAudio( sout_stream_t *p_stream, block_t *p_buffer )
{
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    gcry_error_t i_gcrypt_err;
    block_t *p_next;
    size_t i_len;
    size_t i_payload_len;
    size_t i_realloc_len;
    int rc;
    const uint8_t header[16] = {
        0x24, 0x00, 0x00, 0x00,
        0xf0, 0xff, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
    };
    while ( p_buffer )
    {
        i_len = sizeof( header ) + p_buffer->i_buffer;
        /* Buffer resize needed? */
        if ( i_len > p_sys->i_sendbuf_len || p_sys->p_sendbuf == NULL )
        {
            /* Grow in blocks of 4K */
            i_realloc_len = (1 + (i_len / 4096)) * 4096;
            p_sys->p_sendbuf = realloc_or_free( p_sys->p_sendbuf, i_realloc_len );
            if ( p_sys->p_sendbuf == NULL )
                goto error;
            p_sys->i_sendbuf_len = i_realloc_len;
        }
        /* Fill buffer */
        memcpy( p_sys->p_sendbuf, header, sizeof( header ) );
        memcpy( p_sys->p_sendbuf + sizeof( header ),
                p_buffer->p_buffer, p_buffer->i_buffer );
        /* Calculate payload length and update header */
        i_payload_len = i_len - 4;
        if ( i_payload_len > 0xffff )
        {
            msg_Err( p_stream, "Buffer is too long (%u bytes)",
                     (unsigned int)i_payload_len );
            goto error;
        }
        p_sys->p_sendbuf[2] = ( i_payload_len >> 8 ) & 0xff;
        p_sys->p_sendbuf[3] = i_payload_len & 0xff;
        /* Reset cipher */
        i_gcrypt_err = gcry_cipher_reset( p_sys->aes_ctx );
        if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
            goto error;
        /* Set IV */
        i_gcrypt_err = gcry_cipher_setiv( p_sys->aes_ctx, p_sys->ps_aes_iv,
                                          sizeof( p_sys->ps_aes_iv ) );
        if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
            goto error;
        /* Encrypt in place. Only full blocks of 16 bytes are encrypted,
         * the rest (0-15 bytes) is left unencrypted.
         */
        i_gcrypt_err =
            gcry_cipher_encrypt( p_sys->aes_ctx,
                                 p_sys->p_sendbuf + sizeof( header ),
                                 ( p_buffer->i_buffer / 16 ) * 16,
                                 NULL, 0 );
        if ( CheckForGcryptError( p_stream, i_gcrypt_err ) )
            goto error;
        /* Send data */
        rc = net_Write( p_stream, p_sys->i_stream_fd, NULL,
                        p_sys->p_sendbuf, i_len );
        if ( rc < 0 )
            goto error;
        p_next = p_buffer->p_next;
        block_Release( p_buffer );
        p_buffer = p_next;
    }
error:
    block_ChainRelease( p_buffer );
    return;
}
