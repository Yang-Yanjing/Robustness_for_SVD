static int nut_probe(AVProbeData *p)
{
    int i;
    for (i = 0; i < p->buf_size-8; i++) {
        if (AV_RB32(p->buf+i) != MAIN_STARTCODE>>32)
            continue;
        if (AV_RB32(p->buf+i+4) == (MAIN_STARTCODE & 0xFFFFFFFF))
            return AVPROBE_SCORE_MAX;
    }
    return 0;
}
