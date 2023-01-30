#define FONT_DIR_NT _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Fonts")
static int GetFileFontByName( LPCTSTR font_name, char **psz_filename )
{
    HKEY hKey;
    TCHAR vbuffer[MAX_PATH];
    TCHAR dbuffer[256];
    if( RegOpenKeyEx(HKEY_LOCAL_MACHINE, FONT_DIR_NT, 0, KEY_READ, &hKey)
            != ERROR_SUCCESS )
        return 1;
    char *font_name_temp = FromT( font_name );
    size_t fontname_len = strlen( font_name_temp );
    for( int index = 0;; index++ )
    {
        DWORD vbuflen = MAX_PATH - 1;
        DWORD dbuflen = 255;
        LONG i_result = RegEnumValue( hKey, index, vbuffer, &vbuflen,
                                      NULL, NULL, (LPBYTE)dbuffer, &dbuflen);
        if( i_result != ERROR_SUCCESS )
        {
            RegCloseKey( hKey );
            return i_result;
        }
        char *psz_value = FromT( vbuffer );
        char *s = strchr( psz_value,'(' );
        if( s != NULL && s != psz_value ) s[-1] = '\0';
        /* Manage concatenated font names */
        if( strchr( psz_value, '&') ) {
            if( strcasestr( psz_value, font_name_temp ) != NULL )
            {
                free( psz_value );
                break;
            }
        }
        else {
            if( strncasecmp( psz_value, font_name_temp, fontname_len ) == 0 )
            {
                free( psz_value );
                break;
            }
        }
        free( psz_value );
    }
    *psz_filename = FromT( dbuffer );
    free( font_name_temp );
    RegCloseKey( hKey );
    return 0;
}
