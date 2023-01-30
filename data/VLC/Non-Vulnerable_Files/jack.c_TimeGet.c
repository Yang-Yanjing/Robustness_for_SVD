}
static int TimeGet(audio_output_t *p_aout, mtime_t *delay)
{
    struct aout_sys_t * p_sys = p_aout->sys;
    jack_ringbuffer_t *rb = p_sys->p_jack_ringbuffer;
    const size_t bytes_per_frame = p_sys->i_channels * sizeof(jack_sample_t);
    *delay = (p_sys->latency +
            (jack_ringbuffer_read_space(rb) / bytes_per_frame)) *
        CLOCK_FREQ / p_sys->i_rate;
    return 0;
}
