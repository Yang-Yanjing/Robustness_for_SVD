}
static int Copy(vlc_va_t *va, picture_t *pic, void *opaque, uint8_t *data)
{
    (void) va;
    assert((uintptr_t)opaque == OPAQUE_MAGIC);
    assert((uintptr_t)data == DATA_MAGIC);
    /* Put some dummy picture content */
    memset(pic->p[0].p_pixels, 0xF0,
           pic->p[0].i_pitch * pic->p[0].i_visible_lines);
    for (int i = 0; i < pic->p[1].i_visible_lines; i++)
        memset(pic->p[1].p_pixels + (i * pic->p[1].i_pitch), i,
               pic->p[1].i_visible_pitch);
    for (int i = 0; i < pic->p[2].i_visible_lines; i++)
        for (int j = 0; j < pic->p[2].i_visible_pitch; j++)
            pic->p[2].p_pixels[(i * pic->p[2].i_pitch) + j] = j;
    return VLC_SUCCESS;
}
