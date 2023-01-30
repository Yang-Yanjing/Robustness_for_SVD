}
static inline bool AACCookieChkLen( int64_t i_length, uint64_t i_size, uint64_t i_offset )
{
    return ( i_offset + i_length <= i_size );
}
