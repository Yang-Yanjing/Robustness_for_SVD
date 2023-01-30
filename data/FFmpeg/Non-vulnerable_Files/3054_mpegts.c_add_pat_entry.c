static void add_pat_entry(MpegTSContext *ts, unsigned int programid)
{
    struct Program *p;
    if (av_reallocp_array(&ts->prg, ts->nb_prg + 1, sizeof(*ts->prg)) < 0) {
        ts->nb_prg = 0;
        return;
    }
    p = &ts->prg[ts->nb_prg];
    p->id = programid;
    p->nb_pids = 0;
    p->pmt_found = 0;
    ts->nb_prg++;
}
