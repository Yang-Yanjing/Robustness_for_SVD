}
static void Pause(audio_output_t *p_aout, bool pause, mtime_t date)
{
    VLC_UNUSED(date);
    if (pause) {
        AudioQueuePause(p_aout->sys->audioQueueRef);
        AudioSessionSetActive(false);
    } else {
        AudioQueueStart(p_aout->sys->audioQueueRef, NULL);
        AudioSessionSetActive(true);
    }
}
