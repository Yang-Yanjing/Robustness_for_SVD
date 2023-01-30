}
static int parse_Key(stream_t *s, hls_stream_t *hls, char *p_read)
{
    assert(hls);
    /* #EXT-X-KEY:METHOD=<method>[,URI="<URI>"][,IV=<IV>] */
    int err = VLC_SUCCESS;
    char *attr = parse_Attributes(p_read, "METHOD");
    if (attr == NULL)
    {
        msg_Err(s, "#EXT-X-KEY: expected METHOD=<value>");
        return err;
    }
    if (strncasecmp(attr, "NONE", 4) == 0)
    {
        char *uri = parse_Attributes(p_read, "URI");
        if (uri != NULL)
        {
            msg_Err(s, "#EXT-X-KEY: URI not expected");
            err = VLC_EGENERIC;
        }
        free(uri);
        /* IV is only supported in version 2 and above */
        if (hls->version >= 2)
        {
            char *iv = parse_Attributes(p_read, "IV");
            if (iv != NULL)
            {
                msg_Err(s, "#EXT-X-KEY: IV not expected");
                err = VLC_EGENERIC;
            }
            free(iv);
        }
    }
    else if (strncasecmp(attr, "AES-128", 7) == 0)
    {
        char *value, *uri, *iv;
        if (s->p_sys->b_aesmsg == false)
        {
            msg_Dbg(s, "playback of AES-128 encrypted HTTP Live media detected.");
            s->p_sys->b_aesmsg = true;
        }
        value = uri = parse_Attributes(p_read, "URI");
        if (value == NULL)
        {
            msg_Err(s, "#EXT-X-KEY: URI not found for encrypted HTTP Live media in AES-128");
            free(attr);
            return VLC_EGENERIC;
        }
        /* Url is put between quotes, remove them */
        if (*value == '"')
        {
            /* We need to strip the "" from the attribute value */
            uri = value + 1;
            char* end = strchr(uri, '"');
            if (end != NULL)
                *end = 0;
        }
        /* For absolute URI, just duplicate it
         * don't limit to HTTP, maybe some sanity checking
         * should be done more in here? */
        if( strstr( uri , "://" ) )
            hls->psz_current_key_path = strdup( uri );
        else
            hls->psz_current_key_path = relative_URI(hls->url, uri);
        free(value);
        value = iv = parse_Attributes(p_read, "IV");
        if (iv == NULL)
        {
            /*
            * If the EXT-X-KEY tag does not have the IV attribute, implementations
            * MUST use the sequence number of the media file as the IV when
            * encrypting or decrypting that media file.  The big-endian binary
            * representation of the sequence number SHALL be placed in a 16-octet
            * buffer and padded (on the left) with zeros.
            */
            hls->b_iv_loaded = false;
        }
        else
        {
            /*
            * If the EXT-X-KEY tag has the IV attribute, implementations MUST use
            * the attribute value as the IV when encrypting or decrypting with that
            * key.  The value MUST be interpreted as a 128-bit hexadecimal number
            * and MUST be prefixed with 0x or 0X.
            */
            if (string_to_IV(iv, hls->psz_AES_IV) == VLC_EGENERIC)
            {
                msg_Err(s, "IV invalid");
                err = VLC_EGENERIC;
            }
            else
                hls->b_iv_loaded = true;
            free(value);
        }
    }
    else
    {
        msg_Warn(s, "playback of encrypted HTTP Live media is not supported.");
        err = VLC_EGENERIC;
    }
    free(attr);
    return err;
}
