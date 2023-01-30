}
static bool Eia608ParseExtended( eia608_t *h, uint8_t d1, uint8_t d2 )
{
    assert( d2 >= 0x20 && d2 <= 0x3f );
    assert( d1 == 0x12 || d1 == 0x13 );
    if( d1 == 0x12 )
        d2 += 0x70; /* We use charaters 0x90-0xaf */
    else
        d2 += 0x90; /* We use charaters 0xb0-0xcf */
    /* The extended characters replace the previous one with a more
     * advanced one */
    Eia608Cursor( h, -1 );
    Eia608Write( h, d2 );
    return true;
}
