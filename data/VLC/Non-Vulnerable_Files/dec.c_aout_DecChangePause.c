}
void aout_DecChangePause (audio_output_t *aout, bool paused, mtime_t date)
{
    aout_owner_t *owner = aout_owner (aout);
    aout_OutputLock (aout);
    if (owner->sync.end != VLC_TS_INVALID)
    {
        if (paused)
            owner->sync.end -= date;
        else
            owner->sync.end += date;
    }
    if (owner->mixer_format.i_format)
        aout_OutputPause (aout, paused, date);
    aout_OutputUnlock (aout);
}
