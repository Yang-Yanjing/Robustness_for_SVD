 *****************************************************************************/
static inline unsigned gesture( unsigned i_pattern, unsigned i_num )
{
    return ( i_pattern >> ( i_num * 4 ) ) & 0xF;
}
