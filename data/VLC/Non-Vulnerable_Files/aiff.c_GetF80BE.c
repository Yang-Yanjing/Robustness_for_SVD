/* GetF80BE: read a 80 bits float in big endian */
static unsigned int GetF80BE( const uint8_t p[10] )
{
    unsigned int i_mantissa = GetDWBE( &p[2] );
    int          i_exp = 30 - p[1];
    unsigned int i_last = 0;
    while( i_exp-- > 0 )
    {
        i_last = i_mantissa;
        i_mantissa >>= 1;
    }
    if( i_last&0x01 )
    {
        i_mantissa++;
    }
    return i_mantissa;
}
