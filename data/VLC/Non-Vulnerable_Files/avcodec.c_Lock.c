}
static int Lock(vlc_va_t *va, void **opaque, uint8_t **data)
{
    vlc_vdp_video_field_t *field;
    unsigned tries = (CLOCK_FREQ + VOUT_OUTMEM_SLEEP) / VOUT_OUTMEM_SLEEP;
    while ((field = GetSurface(va)) == NULL)
    {
        if (--tries == 0)
            return VLC_ENOMEM;
        /* Out of video RAM, wait for some time as in src/input/decoder.c.
         * XXX: Both this and the core should use a semaphore or a CV. */
        msleep(VOUT_OUTMEM_SLEEP);
    }
    *opaque = field;
    *data = (void *)(uintptr_t)field->frame->surface;
    return VLC_SUCCESS;
}
