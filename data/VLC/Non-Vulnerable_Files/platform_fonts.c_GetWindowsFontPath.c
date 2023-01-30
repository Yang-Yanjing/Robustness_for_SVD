}
static char* GetWindowsFontPath()
{
    wchar_t wdir[MAX_PATH];
    if( S_OK != SHGetFolderPathW( NULL, CSIDL_FONTS, NULL, SHGFP_TYPE_CURRENT, wdir ) )
    {
        GetWindowsDirectoryW( wdir, MAX_PATH );
        wcscat( wdir, L"\\fonts" );
    }
    return FromWide( wdir );
}
