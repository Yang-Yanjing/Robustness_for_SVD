}
static int TimeGet(audio_output_t *p_aout, mtime_t *restrict delay)
{
    AudioTimeStamp outTimeStamp;
    Boolean b_discontinuity;
    OSStatus status = AudioQueueGetCurrentTime(p_aout->sys->audioQueueRef, p_aout->sys->timelineRef, &outTimeStamp, &b_discontinuity);
    if (status != noErr)
        return -1;
    bool b_started = p_aout->sys->b_started;
    if (!b_started)
        return -1;
    if (b_discontinuity) {
        msg_Dbg(p_aout, "detected output discontinuity");
        return -1;
    }
    mtime_t i_pos = (mtime_t) outTimeStamp.mSampleTime * CLOCK_FREQ / p_aout->sys->i_rate;
    if (i_pos > 0) {
        *delay = p_aout->sys->i_played_length - i_pos;
        return 0;
    } else
        return -1;
}
