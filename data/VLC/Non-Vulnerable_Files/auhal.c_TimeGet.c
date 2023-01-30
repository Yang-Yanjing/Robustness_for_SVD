}
static int TimeGet(audio_output_t *p_aout, mtime_t *delay)
{
    struct aout_sys_t * p_sys = p_aout->sys;
    if (!p_sys->i_bytes_per_sample)
        return -1;
    int32_t availableBytes;
    TPCircularBufferTail(&p_sys->circular_buffer, &availableBytes);
    *delay = ((availableBytes / p_sys->i_bytes_per_sample) + p_sys->i_device_latency) * CLOCK_FREQ / p_sys->i_rate;
    return 0;
}
