static int discard_pid(MpegTSContext *ts, unsigned int pid)
{
    int i, j, k;
    int used = 0, discarded = 0;
    struct Program *p;
    

    for (k = 0; k < ts->stream->nb_programs; k++)
        if (ts->stream->programs[k]->discard == AVDISCARD_ALL)
            break;
    if (k == ts->stream->nb_programs)
        return 0;
    for (i = 0; i < ts->nb_prg; i++) {
        p = &ts->prg[i];
        for (j = 0; j < p->nb_pids; j++) {
            if (p->pids[j] != pid)
                continue;
            
            for (k = 0; k < ts->stream->nb_programs; k++) {
                if (ts->stream->programs[k]->id == p->id) {
                    if (ts->stream->programs[k]->discard == AVDISCARD_ALL)
                        discarded++;
                    else
                        used++;
                }
            }
        }
    }
    return !used && discarded;
}
