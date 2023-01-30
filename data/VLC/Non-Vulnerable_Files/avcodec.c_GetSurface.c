}
static vlc_vdp_video_field_t *GetSurface(vlc_va_t *va)
{
    vlc_va_sys_t *sys = va->sys;
    vlc_vdp_video_field_t *f;
    for (unsigned i = 0; (f = sys->pool[i]) != NULL; i++)
    {
        uintptr_t expected = 1;
        if (atomic_compare_exchange_strong(&f->frame->refs, &expected, 2))
        {
            vlc_vdp_video_field_t *field = vlc_vdp_video_copy(f);
            atomic_fetch_sub(&f->frame->refs, 1);
            return field;
        }
    }
    /* All pictures in the pool are referenced. Try to make a new one. */
    return CreateSurface(va);
}
