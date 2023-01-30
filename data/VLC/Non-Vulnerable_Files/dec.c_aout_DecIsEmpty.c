}
bool aout_DecIsEmpty (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    mtime_t now = mdate ();
    bool empty = true;
    aout_OutputLock (aout);
    if (owner->sync.end != VLC_TS_INVALID)
        empty = owner->sync.end <= now;
    if (empty && owner->mixer_format.i_format)
        /* The last PTS has elapsed already. So the underlying audio output
         * buffer should be empty or almost. Thus draining should be fast
         * and will not block the caller too long. */
        aout_OutputFlush (aout, true);
    aout_OutputUnlock (aout);
    return empty;
}
