static int read_probe(AVProbeData *p)
{
    return ff_guidcmp(p->buf, ff_wtv_guid) ? 0 : AVPROBE_SCORE_MAX;
}
