int ff_rtmp_calc_digest_pos(const uint8_t *buf, int off, int mod_val,
                            int add_val)
{
    int i, digest_pos = 0;
    for (i = 0; i < 4; i++)
        digest_pos += buf[i + off];
    digest_pos = digest_pos % mod_val + add_val;
    return digest_pos;
}
