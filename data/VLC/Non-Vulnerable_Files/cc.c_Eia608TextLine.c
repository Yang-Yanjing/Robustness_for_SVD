#define CAT(t) Eia608Strlcat( psz_text, t, i_text_max )
static void Eia608TextLine( struct eia608_screen *screen, char *psz_text, int i_text_max, int i_row, bool b_html )
{
    const uint8_t *p_char = screen->characters[i_row];
    const eia608_color_t *p_color = screen->colors[i_row];
    const eia608_font_t *p_font = screen->fonts[i_row];
    int i_start;
    int i_end;
    int x;
    eia608_color_t last_color = EIA608_COLOR_DEFAULT;
    bool     b_last_italics = false;
    bool     b_last_underline = false;
    char utf8[4];
    /* Search the start */
    i_start = 0;
    /* Ensure we get a monospaced font (required for accurate positioning */
    if( b_html )
        CAT( "<tt>" );
    /* Convert leading spaces to non-breaking so that they don't get
       stripped by the RenderHtml routine as regular whitespace */
    while( i_start < EIA608_SCREEN_COLUMNS && p_char[i_start] == ' ' ) {
        Eia608TextUtf8( utf8, 0x89 );
        CAT( utf8 );
        i_start++;
    }
    /* Search the end */
    i_end = EIA608_SCREEN_COLUMNS-1;
    while( i_end > i_start && p_char[i_end] == ' ' )
        i_end--;
    /* */
    for( x = i_start; x <= i_end; x++ )
    {
        eia608_color_t color = p_color[x];
        bool b_italics = p_font[x] & EIA608_FONT_ITALICS;
        bool b_underline = p_font[x] & EIA608_FONT_UNDERLINE;
        /* */
        if( b_html )
        {
            bool b_close_color, b_close_italics, b_close_underline;
            /* We create the tags font / i / u in that orders */
            b_close_color = color != last_color && last_color != EIA608_COLOR_DEFAULT;
            b_close_italics = !b_italics && b_last_italics;
            b_close_underline = !b_underline && b_last_underline;
            /* Be sure to create valid html */
            b_close_italics |= b_last_italics && b_close_color;
            b_close_underline |= b_last_underline && ( b_close_italics || b_close_color );
            if( b_close_underline )
                CAT( "</u>" );
            if( b_close_italics )
                CAT( "</i>" );
            if( b_close_color )
                CAT( "</font>" );
            if( color != EIA608_COLOR_DEFAULT && color != last_color)
            {
                static const char *ppsz_color[] = {
                    "#ffffff",  // white
                    "#00ff00",  // green
                    "#0000ff",  // blue
                    "#00ffff",  // cyan
                    "#ff0000",  // red
                    "#ffff00",  // yellow
                    "#ff00ff",  // magenta
                    "#ffffff",  // user defined XXX we use white
                };
                CAT( "<font color=\"" );
                CAT( ppsz_color[color] );
                CAT( "\">" );
            }
            if( ( b_close_italics && b_italics ) || ( b_italics && !b_last_italics ) )
                CAT( "<i>" );
            if( ( b_close_underline && b_underline ) || ( b_underline && !b_last_underline ) )
                CAT( "<u>" );
        }
        if( b_html ) {
            /* Escape XML reserved characters
               http://www.w3.org/TR/xml/#syntax */
            switch (p_char[x]) {
            case '>':
                CAT( "&gt;" );
                break;
            case '<':
                CAT( "&lt;" );
                break;
            case '"':
                CAT( "&quot;" );
                break;
            case '\'':
                CAT( "&apos;" );
                break;
            case '&':
                CAT( "&amp;" );
                break;
            default:
                Eia608TextUtf8( utf8, p_char[x] );
                CAT( utf8 );
                break;
            }
        } else {
            Eia608TextUtf8( utf8, p_char[x] );
            CAT( utf8 );
        }
        /* */
        b_last_underline = b_underline;
        b_last_italics = b_italics;
        last_color = color;
    }
    if( b_html )
    {
        if( b_last_underline )
            CAT( "</u>" );
        if( b_last_italics )
            CAT( "</i>" );
        if( last_color != EIA608_COLOR_DEFAULT )
            CAT( "</font>" );
        CAT( "</tt>" );
    }
#undef CAT
}
