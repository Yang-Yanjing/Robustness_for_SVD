 * value. */
static int64_t rtp_init_ts( const vod_media_t *p_media,
                            const char *psz_vod_session )
{
    if (p_media == NULL || psz_vod_session == NULL)
        return mdate();
    uint64_t i_ts_init;
    /* As per RFC 2326, session identifiers are at least 8 bytes long */
    strncpy((char *)&i_ts_init, psz_vod_session, sizeof(uint64_t));
    i_ts_init ^= (uintptr_t)p_media;
    /* Limit the timestamp to 48 bits, this is enough and allows us
     * to stay away from overflows */
    i_ts_init &= 0xFFFFFFFFFFFF;
    return i_ts_init;
}
