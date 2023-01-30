}
static int FindDevicesCallback(vlc_object_t *object, const char *name,
                               char ***values, char ***descs)
{
    enum_context_t ctx;
    ctx.values = xmalloc(sizeof(char *));
    ctx.descs = xmalloc(sizeof(char *));
    ctx.values[0] = strdup("");
    ctx.descs[0] = strdup(_("Default"));
    ctx.count = 1;
    /* Load direct draw DLL */
    HINSTANCE hddraw_dll = LoadLibrary(_T("DDRAW.DLL"));
    if (hddraw_dll != NULL)
    {
        /* Enumerate displays */
        HRESULT (WINAPI *OurDirectDrawEnumerateEx)(LPDDENUMCALLBACKEXA,
                                                   LPVOID, DWORD) =
              (void *)GetProcAddress(hddraw_dll, DIRECTDRAWENUMERATEEX_NAME);
        if (OurDirectDrawEnumerateEx != NULL)
            OurDirectDrawEnumerateEx(DirectXEnumCallback2, &ctx,
                                     DDENUM_ATTACHEDSECONDARYDEVICES);
        FreeLibrary(hddraw_dll);
    }
    VLC_UNUSED(object);
    VLC_UNUSED(name);
    *values = ctx.values;
    *descs = ctx.descs;
    return ctx.count;
}
