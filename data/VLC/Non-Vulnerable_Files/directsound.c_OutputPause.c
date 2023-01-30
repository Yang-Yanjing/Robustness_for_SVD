}
static void OutputPause( audio_output_t *aout, bool pause, mtime_t date )
{
    Pause( &aout->sys->s, pause );
    (void) date;
}
