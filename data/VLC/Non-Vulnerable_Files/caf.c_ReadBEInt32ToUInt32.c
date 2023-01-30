 */
static inline int ReadBEInt32ToUInt32( const uint8_t *p, uint32_t *i_out )
{
    uint32_t i_value = GetDWBE( p );
    if( i_value > INT32_MAX ) return VLC_EGENERIC;
    *i_out = i_value;
    return VLC_SUCCESS;
}
