static int rtp_probe(AVProbeData *p)
{
    if (av_strstart(p->filename, "rtp:", NULL))
        return AVPROBE_SCORE_MAX;
    return 0;
}
