}
static void BufLeToBe( uint8_t *p_out, const uint8_t *p_in, int i_in )
{
    int i;
    for( i = 0; i < i_in/2; i++  )
    {
        p_out[i*2] = p_in[i*2+1];
        p_out[i*2+1] = p_in[i*2];
    }
}
