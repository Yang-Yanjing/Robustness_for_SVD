}
static void Flush(audio_output_t *p_aout, bool wait)
{
    struct aout_sys_t * p_sys = p_aout->sys;
    jack_ringbuffer_t *rb = p_sys->p_jack_ringbuffer;
    /* Sleep if wait was requested */
    if( wait )
    {
        mtime_t delay;
        if (!TimeGet(p_aout, &delay))
            msleep(delay);
    }
    /* reset ringbuffer read and write pointers */
    jack_ringbuffer_reset(rb);
}
