}
static uint32_t dirac_uint( bs_t *p_bs )
{
    uint32_t u_count = 0, u_value = 0;
    while( !bs_eof( p_bs ) && !bs_read( p_bs, 1 ) )
    {
        u_count++;
        u_value <<= 1;
        u_value |= bs_read( p_bs, 1 );
    }
    return (1 << u_count) - 1 + u_value;
}
