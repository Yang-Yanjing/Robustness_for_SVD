 *****************************************************************************/
static char *formatSegmentPath( char *psz_path, uint32_t i_seg, bool b_sanitize )
{
    char *psz_result;
    char *psz_firstNumSign;
    if ( ! ( psz_result  = str_format_time( psz_path ) ) )
        return NULL;
    psz_firstNumSign = psz_result + strcspn( psz_result, SEG_NUMBER_PLACEHOLDER );
    if ( *psz_firstNumSign )
    {
        char *psz_newResult;
        int i_cnt = strspn( psz_firstNumSign, SEG_NUMBER_PLACEHOLDER );
        int ret;
        *psz_firstNumSign = '\0';
        ret = asprintf( &psz_newResult, "%s%0*d%s", psz_result, i_cnt, i_seg, psz_firstNumSign + i_cnt );
        free ( psz_result );
        if ( ret < 0 )
            return NULL;
        psz_result = psz_newResult;
    }
    if ( b_sanitize )
        path_sanitize( psz_result );
    return psz_result;
}
