}
static int Buf14To16( uint8_t *p_out, const uint8_t *p_in, int i_in, int i_le )
{
    unsigned char tmp, cur = 0;
    int bits_in, bits_out = 0;
    int i, i_out = 0;
    for( i = 0; i < i_in; i++  )
    {
        if( i%2 )
        {
            tmp = p_in[i-i_le];
            bits_in = 8;
        }
        else
        {
            tmp = p_in[i+i_le] & 0x3F;
            bits_in = 8 - 2;
        }
        if( bits_out < 8 )
        {
            int need = __MIN( 8 - bits_out, bits_in );
            cur <<= need;
            cur |= ( tmp >> (bits_in - need) );
            tmp <<= (8 - bits_in + need);
            tmp >>= (8 - bits_in + need);
            bits_in -= need;
            bits_out += need;
        }
        if( bits_out == 8 )
        {
            p_out[i_out] = cur;
            cur = 0;
            bits_out = 0;
            i_out++;
        }
        bits_out += bits_in;
        cur <<= bits_in;
        cur |= tmp;
    }
    return i_out;
}
