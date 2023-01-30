}
static void DxDestroyVideoConversion(vlc_va_sys_t *va)
{
    CopyCleanCache(&va->surface_cache);
}
