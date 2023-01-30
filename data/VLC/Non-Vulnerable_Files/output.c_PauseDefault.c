}
static void PauseDefault (audio_output_t *aout, bool pause, mtime_t date)
{
    if (pause)
        aout_OutputFlush (aout, false);
    (void) date;
}
