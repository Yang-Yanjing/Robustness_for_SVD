}
static void Flush(audio_output_t *p_aout, bool wait)
{
    if (!p_aout->sys->audioQueueRef)
        return;
    AudioQueueDisposeTimeline(p_aout->sys->audioQueueRef, p_aout->sys->timelineRef);
    if (wait)
        AudioQueueStop(p_aout->sys->audioQueueRef, false);
    else
        AudioQueueStop(p_aout->sys->audioQueueRef, true);
    p_aout->sys->b_started = false;
    p_aout->sys->i_played_length = 0;
    AudioQueueStart(p_aout->sys->audioQueueRef, NULL);
    AudioQueueCreateTimeline(p_aout->sys->audioQueueRef, &p_aout->sys->timelineRef);
    p_aout->sys->b_started = true;
}
