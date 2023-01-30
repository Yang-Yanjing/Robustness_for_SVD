}
static bool Eia608ParseCommand0x17( eia608_t *h, uint8_t d2 )
{
    switch( d2 )
    {
    case 0x21:  /* Tab offset 1 */
        Eia608Cursor( h, 1 );
        break;
    case 0x22:  /* Tab offset 2 */
        Eia608Cursor( h, 2 );
        break;
    case 0x23:  /* Tab offset 3 */
        Eia608Cursor( h, 3 );
        break;
    }
    return false;
}
