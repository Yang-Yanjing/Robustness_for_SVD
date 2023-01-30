}
static int TimeGet(audio_output_t* aout, mtime_t* restrict drift)
{
    aout_sys_t *sys = aout->sys;
    SLAndroidSimpleBufferQueueState st;
    SLresult res = GetState(sys->playerBufferQueue, &st);
    if (unlikely(res != SL_RESULT_SUCCESS)) {
        msg_Err(aout, "Could not query buffer queue state in TimeGet (%lu)", res);
        return -1;
    }
    vlc_mutex_lock(&sys->lock);
    bool started = sys->started;
    vlc_mutex_unlock(&sys->lock);
    if (!started)
        return -1;
    *drift = (CLOCK_FREQ * OPENSLES_BUFLEN * st.count / 1000)
        + sys->samples * CLOCK_FREQ / sys->rate;
    /* msg_Dbg(aout, "latency %"PRId64" ms, %d/%d buffers", *drift / 1000,
        (int)st.count, OPENSLES_BUFFERS); */
    return 0;
}
