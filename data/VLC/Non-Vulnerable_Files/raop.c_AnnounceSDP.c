}
static int AnnounceSDP( vlc_object_t *p_this, char *psz_local,
                        uint32_t i_session_id )
{
    sout_stream_t *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t *p_sys = p_stream->p_sys;
    vlc_dictionary_t req_headers;
    vlc_dictionary_t resp_headers;
    unsigned char ps_sac[16];
    char *psz_sdp = NULL;
    char *psz_sac_base64 = NULL;
    char *psz_aes_key_base64 = NULL;
    char *psz_aes_iv_base64 = NULL;
    int i_err = VLC_SUCCESS;
    int i_rc;
    vlc_dictionary_init( &req_headers, 0 );
    vlc_dictionary_init( &resp_headers, 0 );
    /* Encrypt AES key and encode it in Base64 */
    i_rc = EncryptAesKeyBase64( p_this, &psz_aes_key_base64 );
    if ( i_rc != VLC_SUCCESS || psz_aes_key_base64 == NULL )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    RemoveBase64Padding( psz_aes_key_base64 );
    /* Encode AES IV in Base64 */
    psz_aes_iv_base64 = vlc_b64_encode_binary( p_sys->ps_aes_iv,
                                               sizeof( p_sys->ps_aes_iv ) );
    if ( psz_aes_iv_base64 == NULL )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    RemoveBase64Padding( psz_aes_iv_base64 );
    /* Random bytes for Apple-Challenge header */
    gcry_randomize( ps_sac, sizeof( ps_sac ), GCRY_STRONG_RANDOM );
    psz_sac_base64 = vlc_b64_encode_binary( ps_sac, sizeof( ps_sac ) );
    if ( psz_sac_base64 == NULL )
    {
        i_err = VLC_EGENERIC;
        goto error;
    }
    RemoveBase64Padding( psz_sac_base64 );
    /* Build SDP
     * Note: IPv6 addresses also use "IP4". Make sure not to include the
     * scope ID.
     */
    i_rc = asprintf( &psz_sdp,
                     "v=0\r\n"
                     "o=iTunes %u 0 IN IP4 %s\r\n"
                     "s=iTunes\r\n"
                     "c=IN IP4 %s\r\n"
                     "t=0 0\r\n"
                     "m=audio 0 RTP/AVP 96\r\n"
                     "a=rtpmap:96 AppleLossless\r\n"
                     "a=fmtp:96 4096 0 16 40 10 14 2 255 0 0 44100\r\n"
                     "a=rsaaeskey:%s\r\n"
                     "a=aesiv:%s\r\n",
                     i_session_id, psz_local, p_sys->psz_host,
                     psz_aes_key_base64, psz_aes_iv_base64 );
    if ( i_rc < 0 )
    {
        i_err = VLC_ENOMEM;
        goto error;
    }
    /* Build and send request */
    vlc_dictionary_insert( &req_headers, "Apple-Challenge", psz_sac_base64 );
    i_err = ExecRequest( p_this, "ANNOUNCE", "application/sdp", psz_sdp,
                         &req_headers, &resp_headers);
    if ( i_err != VLC_SUCCESS )
        goto error;
error:
    vlc_dictionary_clear( &req_headers, NULL, NULL );
    vlc_dictionary_clear( &resp_headers, FreeHeader, NULL );
    free( psz_sdp );
    free( psz_sac_base64 );
    free( psz_aes_key_base64 );
    free( psz_aes_iv_base64 );
    return i_err;
}
