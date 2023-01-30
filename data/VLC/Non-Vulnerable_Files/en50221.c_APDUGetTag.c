 *****************************************************************************/
static int APDUGetTag( const uint8_t *p_apdu, int i_size )
{
    if ( i_size >= 3 )
    {
        int i, t = 0;
        for ( i = 0; i < 3; i++ )
            t = (t << 8) | *p_apdu++;
        return t;
    }
    return AOT_NONE;
}
