static int probe(AVProbeData *p, int live)
{
    const uint8_t *d = p->buf;
    unsigned offset = AV_RB32(d + 5);
    if (d[0] == 'F' &&
        d[1] == 'L' &&
        d[2] == 'V' &&
        d[3] < 5 && d[5] == 0 &&
        offset + 100 < p->buf_size &&
        offset > 8) {
        int is_live = !memcmp(d + offset + 40, "NGINX RTMP", 10);
        if (live == is_live)
            return AVPROBE_SCORE_MAX;
    }
    return 0;
}
