}
int aout_OutputTimeGet (audio_output_t *aout, mtime_t *delay)
{
    aout_OutputAssertLocked (aout);
    if (aout->time_get == NULL)
        return -1;
    return aout->time_get (aout, delay);
}
