}
static bool Eia608ParsePac( eia608_t *h, uint8_t d1, uint8_t d2 )
{
    static const int pi_row[] = {
        11, -1, 1, 2, 3, 4, 12, 13, 14, 15, 5, 6, 7, 8, 9, 10
    };
    const int i_row_index = ( (d1<<1) & 0x0e) | ( (d2>>5) & 0x01 );
    assert( d2 >= 0x40 && d2 <= 0x7f );
    if( pi_row[i_row_index] <= 0 )
        return false;
    /* Row */
    if( h->mode != EIA608_MODE_TEXT )
        h->cursor.i_row = pi_row[i_row_index] - 1;
    h->i_row_rollup = pi_row[i_row_index] - 1;
    /* Column */
    if( d2 >= 0x60 )
        d2 -= 0x60;
    else if( d2 >= 0x40 )
        d2 -= 0x40;
    h->cursor.i_column = pac2_attribs[d2].i_column;
    h->color = pac2_attribs[d2].i_color;
    h->font  = pac2_attribs[d2].i_font;
    return false;
}
