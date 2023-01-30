}
static void CreateDecodedNAL( uint8_t **pp_ret, int *pi_ret,
                              const uint8_t *src, int i_src )
{
    const uint8_t *end = &src[i_src];
    uint8_t *dst = malloc( i_src );
    *pp_ret = dst;
    if( dst )
    {
        while( src < end )
        {
            if( src < end - 3 && src[0] == 0x00 && src[1] == 0x00 &&
                src[2] == 0x03 )
            {
                *dst++ = 0x00;
                *dst++ = 0x00;
                src += 3;
                continue;
            }
            *dst++ = *src++;
        }
    }
    *pi_ret = dst - *pp_ret;
}
