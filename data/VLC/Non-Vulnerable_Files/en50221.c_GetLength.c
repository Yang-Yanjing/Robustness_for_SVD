#define SIZE_INDICATOR 0x80
static uint8_t *GetLength( uint8_t *p_data, int *pi_length )
{
    *pi_length = *p_data++;
    if ( (*pi_length & SIZE_INDICATOR) != 0 )
    {
        int l = *pi_length & ~SIZE_INDICATOR;
        int i;
        *pi_length = 0;
        for ( i = 0; i < l; i++ )
            *pi_length = (*pi_length << 8) | *p_data++;
    }
    return p_data;
}
