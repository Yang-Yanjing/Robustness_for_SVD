}
static bool Eia608ParseTextAttribute( eia608_t *h, uint8_t d2 )
{
    const int i_index = d2 - 0x20;
    assert( d2 >= 0x20 && d2 <= 0x2f );
    h->color = pac2_attribs[i_index].i_color;
    h->font  = pac2_attribs[i_index].i_font;
    Eia608Cursor( h, 1 );
    return false;
}
