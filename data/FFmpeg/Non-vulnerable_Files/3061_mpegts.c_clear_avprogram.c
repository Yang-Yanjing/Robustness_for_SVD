static void clear_avprogram(MpegTSContext *ts, unsigned int programid)
{
    AVProgram *prg = NULL;
    int i;
    for (i = 0; i < ts->stream->nb_programs; i++)
        if (ts->stream->programs[i]->id == programid) {
            prg = ts->stream->programs[i];
            break;
        }
    if (!prg)
        return;
    prg->nb_stream_indexes = 0;
}
