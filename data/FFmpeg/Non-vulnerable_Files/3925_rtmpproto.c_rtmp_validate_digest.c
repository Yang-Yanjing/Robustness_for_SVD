static int rtmp_validate_digest(uint8_t *buf, int off)
{
    uint8_t digest[32];
    int ret, digest_pos;
    digest_pos = ff_rtmp_calc_digest_pos(buf, off, 728, off + 4);
    ret = ff_rtmp_calc_digest(buf, RTMP_HANDSHAKE_PACKET_SIZE, digest_pos,
                              rtmp_server_key, SERVER_KEY_OPEN_PART_LEN,
                              digest);
    if (ret < 0)
        return ret;
    if (!memcmp(digest, buf + digest_pos, 32))
        return digest_pos;
    return 0;
}
