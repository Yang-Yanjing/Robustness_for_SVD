}
static int hls_DownloadSegmentKey(stream_t *s, segment_t *seg)
{
    stream_t *p_m3u8 = stream_UrlNew(s, seg->psz_key_path);
    if (p_m3u8 == NULL)
    {
        msg_Err(s, "Failed to load the AES key for segment sequence %d", seg->sequence);
        return VLC_EGENERIC;
    }
    int len = stream_Read(p_m3u8, seg->aes_key, sizeof(seg->aes_key));
    stream_Delete(p_m3u8);
    if (len != AES_BLOCK_SIZE)
    {
        msg_Err(s, "The AES key loaded doesn't have the right size (%d)", len);
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}
