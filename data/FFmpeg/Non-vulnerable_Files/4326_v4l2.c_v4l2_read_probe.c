static int v4l2_read_probe(AVProbeData *p)
{
    if (av_strstart(p->filename, "/dev/video", NULL))
        return AVPROBE_SCORE_MAX - 1;
    return 0;
}
