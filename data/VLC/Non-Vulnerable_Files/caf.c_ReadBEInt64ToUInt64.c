*/
static inline int ReadBEInt64ToUInt64( const uint8_t *p, uint64_t *i_out )
{
    uint64_t i_value = GetQWBE( p );
    if( i_value > INT64_MAX ) return VLC_EGENERIC;
    *i_out = i_value;
    return VLC_SUCCESS;
}
