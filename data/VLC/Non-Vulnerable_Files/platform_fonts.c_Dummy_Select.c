#endif
char* Dummy_Select( filter_t *p_filter, const char* psz_font,
                    bool b_bold, bool b_italic, int i_size, int *i_idx )
{
    VLC_UNUSED(p_filter);
    VLC_UNUSED(b_bold);
    VLC_UNUSED(b_italic);
    VLC_UNUSED(i_size);
    VLC_UNUSED(i_idx);
    char *psz_fontname;
# if defined( _WIN32 ) && !VLC_WINSTORE_APP
    /* Get Windows Font folder */
    char *psz_win_fonts_path = GetWindowsFontPath();
    if( asprintf( &psz_fontname, "%s\\%s", psz_win_fonts_path, psz_font ) == -1 )
    {
        psz_fontname = NULL;
        return NULL;
    }
    free(psz_win_fonts_path);
# else
    psz_fontname = strdup( psz_font );
# endif
    return psz_fontname;
}
