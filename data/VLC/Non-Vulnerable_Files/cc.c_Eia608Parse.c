}
static bool Eia608Parse( eia608_t *h, int i_channel_selected, const uint8_t data[2] )
{
    const uint8_t d1 = data[0] & 0x7f; /* Removed parity bit */
    const uint8_t d2 = data[1] & 0x7f;
    bool b_screen_changed = false;
    if( d1 == 0 && d2 == 0 )
        return false;   /* Ignore padding (parity check are sometimes invalid on them) */
    Eia608ParseChannel( h, data );
    if( h->i_channel != i_channel_selected )
        return false;
    //fprintf( stderr, "CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC %x %x\n", data[0], data[1] );
    if( d1 >= 0x10 )
    {
        if( d1 >= 0x20 ||
            d1 != h->last.d1 || d2 != h->last.d2 ) /* Command codes can be repeated */
            b_screen_changed = Eia608ParseData( h, d1,d2 );
        h->last.d1 = d1;
        h->last.d2 = d2;
    }
    else if( ( d1 >= 0x01 && d1 <= 0x0E ) || d1 == 0x0F )
    {
        /* XDS block / End of XDS block */
    }
    return b_screen_changed;
}
