}
static void Eia608Erase( eia608_t *h )
{
    const int i_row = h->cursor.i_row;
    const int i_column = h->cursor.i_column - 1;
    eia608_screen *screen;
    if( h->mode == EIA608_MODE_TEXT )
        return;
    if( i_column < 0 )
        return;
    screen = &h->screen[Eia608GetWritingScreenIndex( h )];
    /* FIXME do we need to reset row_used/colors/font ? */
    screen->characters[i_row][i_column] = ' ';
    Eia608Cursor( h, -1 );
}
