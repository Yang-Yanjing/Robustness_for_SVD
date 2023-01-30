}
static void Eia608ClearScreenRowX( eia608_t *h, int i_screen, int i_row, int x )
{
    eia608_screen *screen = &h->screen[i_screen];
    if( x == 0 )
    {
        screen->row_used[i_row] = false;
    }
    else
    {
        screen->row_used[i_row] = false;
        for( int i = 0; i < x; i++ )
        {
            if( screen->characters[i_row][i] != ' ' ||
                screen->colors[i_row][i] != EIA608_COLOR_DEFAULT ||
                screen->fonts[i_row][i] != EIA608_FONT_REGULAR )
            {
                screen->row_used[i_row] = true;
                break;
            }
        }
    }
    for( ; x < EIA608_SCREEN_COLUMNS+1; x++ )
    {
        screen->characters[i_row][x] = x < EIA608_SCREEN_COLUMNS ? ' ' : '\0';
        screen->colors[i_row][x] = EIA608_COLOR_DEFAULT;
        screen->fonts[i_row][x] = EIA608_FONT_REGULAR;
    }
}
