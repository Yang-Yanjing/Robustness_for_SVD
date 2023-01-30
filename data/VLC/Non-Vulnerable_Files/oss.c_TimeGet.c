}
static int TimeGet (audio_output_t *aout, mtime_t *restrict pts)
{
    aout_sys_t *sys = aout->sys;
    int delay;
    if (ioctl (sys->fd, SNDCTL_DSP_GETODELAY, &delay) < 0)
    {
        msg_Warn (aout, "cannot get delay: %s", vlc_strerror_c(errno));
        return -1;
    }
    *pts = (delay * CLOCK_FREQ * sys->format.i_frame_length)
                        / (sys->format.i_rate * sys->format.i_bytes_per_frame);
    return 0;
}
