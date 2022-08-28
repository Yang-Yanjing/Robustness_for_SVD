static av_cold int sbg_read_probe(AVProbeData *p)
{
    int r, score;
    struct sbg_script script = { 0 };
    r = parse_script(NULL, p->buf, p->buf_size, &script);
    score = r < 0 || !script.nb_def || !script.nb_tseq ? 0 :
            AVPROBE_SCORE_MAX / 3;
    free_script(&script);
    return score;
}
