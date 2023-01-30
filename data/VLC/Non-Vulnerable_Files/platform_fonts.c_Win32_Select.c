}
char* Win32_Select( filter_t *p_filter, const char* family,
                           bool b_bold, bool b_italic, int i_size, int *i_idx )
{
    VLC_UNUSED( i_size );
    VLC_UNUSED( i_idx );
    VLC_UNUSED( p_filter );
    if( !family || strlen( family ) < 1 )
        goto fail;
    /* */
    LOGFONT lf;
    lf.lfCharSet = DEFAULT_CHARSET;
    if( b_italic )
        lf.lfItalic = true;
    if( b_bold )
        lf.lfWeight = FW_BOLD;
    LPTSTR psz_fbuffer = ToT( family );
    _tcsncpy( (LPTSTR)&lf.lfFaceName, psz_fbuffer, LF_FACESIZE );
    free( psz_fbuffer );
    /* */
    char *psz_filename = NULL;
    HDC hDC = GetDC( NULL );
    EnumFontFamiliesEx(hDC, &lf, (FONTENUMPROC)&EnumFontCallback, (LPARAM)&psz_filename, 0);
    ReleaseDC(NULL, hDC);
    /* */
    if( psz_filename != NULL )
    {
        /* FIXME: increase i_idx, when concatenated strings  */
        i_idx = 0;
        /* Prepend the Windows Font path, when only a filename was provided */
        if( strchr( psz_filename, DIR_SEP_CHAR ) )
            return psz_filename;
        else
        {
            /* Get Windows Font folder */
            char *psz_win_fonts_path = GetWindowsFontPath();
            char *psz_tmp;
            if( asprintf( &psz_tmp, "%s\\%s", psz_win_fonts_path, psz_filename ) == -1 )
            {
                free( psz_filename );
                free( psz_win_fonts_path );
                return NULL;
            }
            free( psz_filename );
                free( psz_win_fonts_path );
            return psz_tmp;
        }
    }
    else /* Let's take any font we can */
fail:
    {
        char *psz_win_fonts_path = GetWindowsFontPath();
        char *psz_tmp;
        if( asprintf( &psz_tmp, "%s\\%s", psz_win_fonts_path, SYSTEM_DEFAULT_FONT_FILE ) == -1 )
            return NULL;
        else
            return psz_tmp;
    }
}
