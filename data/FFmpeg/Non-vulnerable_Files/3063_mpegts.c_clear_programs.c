static void clear_programs(MpegTSContext *ts)
{
    av_freep(&ts->prg);
    ts->nb_prg = 0;
}
