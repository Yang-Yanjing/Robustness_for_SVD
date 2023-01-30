}
static void Eia608RollUp( eia608_t *h )
{
    if( h->mode == EIA608_MODE_TEXT )
        return;
    const int i_screen = Eia608GetWritingScreenIndex( h );
    eia608_screen *screen = &h->screen[i_screen];
    int keep_lines;
    /* Window size */
    if( h->mode == EIA608_MODE_ROLLUP_2 )
        keep_lines = 2;
    else if( h->mode == EIA608_MODE_ROLLUP_3 )
        keep_lines = 3;
    else if( h->mode == EIA608_MODE_ROLLUP_4 )
        keep_lines = 4;
    else
        return;
    /* Reset the cursor */
    h->cursor.i_column = 0;
    /* Erase lines above our window */
    for( int i = 0; i < h->cursor.i_row - keep_lines; i++ )
        Eia608ClearScreenRow( h, i_screen, i );
    /* Move up */
    for( int i = 0; i < keep_lines-1; i++ )
    {
        const int i_row = h->cursor.i_row - keep_lines + i + 1;
        if( i_row < 0 )
            continue;
        assert( i_row+1 < EIA608_SCREEN_ROWS );
        memcpy( screen->characters[i_row], screen->characters[i_row+1], sizeof(*screen->characters) );
        memcpy( screen->colors[i_row], screen->colors[i_row+1], sizeof(*screen->colors) );
        memcpy( screen->fonts[i_row], screen->fonts[i_row+1], sizeof(*screen->fonts) );
        screen->row_used[i_row] = screen->row_used[i_row+1];
    }
    /* Reset current row */
    Eia608ClearScreenRow( h, i_screen, h->cursor.i_row );
}
