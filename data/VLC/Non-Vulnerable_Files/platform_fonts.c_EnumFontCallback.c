}
static int CALLBACK EnumFontCallback(const ENUMLOGFONTEX *lpelfe, const NEWTEXTMETRICEX *metric,
                                     DWORD type, LPARAM lParam)
{
    VLC_UNUSED( metric );
    if( (type & RASTER_FONTTYPE) ) return 1;
    // if( lpelfe->elfScript ) FIXME
    return GetFileFontByName( (LPCTSTR)lpelfe->elfFullName, (char **)lParam );
}
