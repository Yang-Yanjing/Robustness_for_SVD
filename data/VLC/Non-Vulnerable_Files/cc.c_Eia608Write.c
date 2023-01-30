}
static void Eia608Write( eia608_t *h, const uint8_t c )
{
    const int i_row = h->cursor.i_row;
    const int i_column = h->cursor.i_column;
    eia608_screen *screen;
    if( h->mode == EIA608_MODE_TEXT )
        return;
    screen = &h->screen[Eia608GetWritingScreenIndex( h )];
    screen->characters[i_row][i_column] = c;
    screen->colors[i_row][i_column] = h->color;
    screen->fonts[i_row][i_column] = h->font;
    screen->row_used[i_row] = true;
    Eia608Cursor( h, 1 );
}
