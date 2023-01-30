}
static void Flush (audio_output_t *p_aout, bool wait)
{
    aout_sys_t *p_sys = p_aout->sys;
    if (wait) {
        mtime_t delay;
        if (!TimeGet(p_aout, &delay))
            msleep(delay);
    } else {
        p_sys->at_stop(p_sys->AudioTrack);
        p_sys->at_flush(p_sys->AudioTrack);
        p_sys->samples_written = 0;
        p_sys->at_start(p_sys->AudioTrack);
    }
}
