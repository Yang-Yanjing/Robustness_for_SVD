#define EIA608_COLOR_DEFAULT EIA608_COLOR_WHITE
static void Eia608Cursor( eia608_t *h, int dx )
{
    h->cursor.i_column += dx;
    if( h->cursor.i_column < 0 )
        h->cursor.i_column = 0;
    else if( h->cursor.i_column > EIA608_SCREEN_COLUMNS-1 )
        h->cursor.i_column = EIA608_SCREEN_COLUMNS-1;
}
