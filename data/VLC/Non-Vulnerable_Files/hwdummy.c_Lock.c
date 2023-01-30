#define OPAQUE_MAGIC  0x0da00e00
static int Lock(vlc_va_t *va, void **opaque, uint8_t **data)
{
    *data = (void *)(uintptr_t)DATA_MAGIC;
    *opaque = (void *)(uintptr_t)OPAQUE_MAGIC;
    (void) va;
    return VLC_SUCCESS;
}
