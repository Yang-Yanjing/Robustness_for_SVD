}
static void DirectXUnlock(picture_t *picture)
{
    DirectXUnlockSurface(picture->p_sys->front_surface,
                         picture->p_sys->surface);
}
