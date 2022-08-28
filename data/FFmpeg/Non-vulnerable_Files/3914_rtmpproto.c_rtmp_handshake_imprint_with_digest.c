static int rtmp_handshake_imprint_with_digest(uint8_t *buf, int encrypted)
{
    int ret, digest_pos;
    if (encrypted)
        digest_pos = ff_rtmp_calc_digest_pos(buf, 772, 728, 776);
    else
        digest_pos = ff_rtmp_calc_digest_pos(buf, 8, 728, 12);
    ret = ff_rtmp_calc_digest(buf, RTMP_HANDSHAKE_PACKET_SIZE, digest_pos,
                              rtmp_player_key, PLAYER_KEY_OPEN_PART_LEN,
                              buf + digest_pos);
    if (ret < 0)
        return ret;
    return digest_pos;
}
