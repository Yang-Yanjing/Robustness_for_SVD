}
static void Release(void *opaque, uint8_t *data)
{
    vlc_va_surface_t *surface = opaque;
    surface->refcount--;
    (void) data;
}
