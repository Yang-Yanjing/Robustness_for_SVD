}
static bool Eia608ParseData( eia608_t *h, uint8_t d1, uint8_t d2 )
{
    bool b_changed = false;
    if( d1 >= 0x18 && d1 <= 0x1f )
        d1 -= 8;
#define ON( d2min, d2max, cmd ) do { if( d2 >= d2min && d2 <= d2max ) b_changed = cmd; } while(0)
    switch( d1 )
    {
    case 0x11:
        ON( 0x20, 0x2f, Eia608ParseTextAttribute( h, d2 ) );
        ON( 0x30, 0x3f, Eia608ParseDouble( h, d2 ) );
        break;
    case 0x12: case 0x13:
        ON( 0x20, 0x3f, Eia608ParseExtended( h, d1, d2 ) );
        break;
    case 0x14: case 0x15:
        ON( 0x20, 0x2f, Eia608ParseCommand0x14( h, d2 ) );
        break;
    case 0x17:
        ON( 0x21, 0x23, Eia608ParseCommand0x17( h, d2 ) );
        ON( 0x2e, 0x2f, Eia608ParseTextAttribute( h, d2 ) );
        break;
    }
    if( d1 == 0x10 )
        ON( 0x40, 0x5f, Eia608ParsePac( h, d1, d2 ) );
    else if( d1 >= 0x11 && d1 <= 0x17 )
        ON( 0x40, 0x7f, Eia608ParsePac( h, d1, d2 ) );
#undef ON
    if( d1 >= 0x20 )
    {
        b_changed = Eia608ParseSingle( h, d1 );
        if( d2 >= 0x20 )
            b_changed |= Eia608ParseSingle( h, d2 );
    }
    return b_changed;
}
