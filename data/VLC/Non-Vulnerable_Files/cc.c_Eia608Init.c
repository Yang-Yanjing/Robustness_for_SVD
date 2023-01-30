/* */
static void Eia608Init( eia608_t *h )
{
    memset( h, 0, sizeof(*h) );
    /* */
    h->i_channel = -1;
    h->i_screen = 0;
    Eia608ClearScreen( h, 0 );
    Eia608ClearScreen( h, 1 );
    /* Cursor for writing text */
    h->cursor.i_column = 0;
    h->cursor.i_row = 0;
    h->last.d1 = 0x00;
    h->last.d2 = 0x00;
    h->mode = EIA608_MODE_POPUP;
    h->color = EIA608_COLOR_DEFAULT;
    h->font = EIA608_FONT_REGULAR;
    h->i_row_rollup = EIA608_SCREEN_ROWS-1;
}
