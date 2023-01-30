 */
void aout_OutputPause( audio_output_t *aout, bool pause, mtime_t date )
{
    aout_OutputAssertLocked (aout);
    ((aout->pause != NULL) ? aout->pause : PauseDefault) (aout, pause, date);
}
