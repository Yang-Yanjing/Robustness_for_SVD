 */
static int ReloadDirectXDevices( vlc_object_t *p_this, char const *psz_name,
                                 char ***values, char ***descs )
{
    ds_list_t list = {
        .count = 1,
        .ids = xmalloc(sizeof (char *)),
        .names = xmalloc(sizeof (char *)),
    };
    list.ids[0] = xstrdup("");
    list.names[0] = xstrdup(_("Default"));
    (void) psz_name;
    HANDLE hdsound_dll = LoadLibrary(_T("DSOUND.DLL"));
    if( hdsound_dll == NULL )
    {
        msg_Warn( p_this, "cannot open DSOUND.DLL" );
        goto out;
    }
    /* Get DirectSoundEnumerate */
    HRESULT (WINAPI *OurDirectSoundEnumerate)(LPDSENUMCALLBACKW, LPVOID) =
            (void *)GetProcAddress( hdsound_dll, "DirectSoundEnumerateW" );
    if( OurDirectSoundEnumerate != NULL )
    {
        OurDirectSoundEnumerate( DeviceEnumCallback, &list );
        msg_Dbg( p_this, "found %u devices", list.count );
    }
    FreeLibrary(hdsound_dll);
out:
    *values = list.ids;
    *descs = list.names;
    return list.count;
}
