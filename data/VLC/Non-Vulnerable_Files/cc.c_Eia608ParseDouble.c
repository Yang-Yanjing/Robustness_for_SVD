}
static bool Eia608ParseDouble( eia608_t *h, uint8_t d2 )
{
    assert( d2 >= 0x30 && d2 <= 0x3f );
    Eia608Write( h, d2 + 0x50 ); /* We use charaters 0x80...0x8f */
    return true;
}
