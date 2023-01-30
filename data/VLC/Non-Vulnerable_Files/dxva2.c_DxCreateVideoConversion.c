}
static void DxCreateVideoConversion(vlc_va_sys_t *va)
{
    switch (va->render) {
    case MAKEFOURCC('N','V','1','2'):
    case MAKEFOURCC('I','M','C','3'):
        va->output = MAKEFOURCC('Y','V','1','2');
        break;
    default:
        va->output = va->render;
        break;
    }
    CopyInitCache(&va->surface_cache, va->surface_width);
}
