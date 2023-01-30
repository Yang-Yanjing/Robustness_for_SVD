}
static void Stop(audio_output_t *p_aout)
{
    AudioSessionSetActive(false);
    p_aout->sys->i_played_length = 0;
    AudioQueueDisposeTimeline(p_aout->sys->audioQueueRef, p_aout->sys->timelineRef);
    AudioQueueStop(p_aout->sys->audioQueueRef, true);
    AudioQueueDispose(p_aout->sys->audioQueueRef, true);
    msg_Dbg(p_aout, "audioqueue stopped and disposed");
}
