 *****************************************************************************/
static inline unsigned int AddNibble( unsigned int i_code,
                                      const uint8_t *p_src, unsigned int *pi_index )
{
    if( *pi_index & 0x1 )
    {
        return( i_code << 4 | ( p_src[(*pi_index)++ >> 1] & 0xf ) );
    }
    else
    {
        return( i_code << 4 | p_src[(*pi_index)++ >> 1] >> 4 );
    }
}
