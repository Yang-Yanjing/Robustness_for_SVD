}
static char *Eia608Text( eia608_t *h, bool b_html )
{
    const int i_size = EIA608_SCREEN_ROWS * 10 * EIA608_SCREEN_COLUMNS+1;
    struct eia608_screen *screen = &h->screen[h->i_screen];
    bool b_first = true;
    char *psz;
    /* We allocate a buffer big enough for normal case */
    psz = malloc( i_size );
    if( !psz )
        return NULL;
    *psz = '\0';
    if( b_html )
        Eia608Strlcat( psz, "<text>", i_size );
    for( int i = 0; i < EIA608_SCREEN_ROWS; i++ )
    {
        if( !b_first )
            Eia608Strlcat( psz, b_html ? "<br />" : "\n", i_size );
        b_first = false;
        Eia608TextLine( screen, psz, i_size, i, b_html );
    }
    if( b_html )
        Eia608Strlcat( psz, "</text>", i_size );
    return psz;
}
