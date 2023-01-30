 *****************************************************************************/
static bool NumInRange( const char *psz_range, int i_num )
{
    const char *psz = strchr( psz_range, '-' );
    char *end;
    int  i_start, i_stop;
    i_start = strtol( psz_range, &end, 0 );
    if( end == psz_range )
        i_start = i_num;
    if( psz )
    {
        psz++;
        i_stop = strtol( psz, &end, 0 );
        if( end == psz )
            i_stop = i_num;
    }
    else
        i_stop = i_start;
    return i_start <= i_num && i_num <= i_stop;
}
