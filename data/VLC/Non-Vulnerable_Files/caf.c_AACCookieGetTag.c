/* Helper functions for AAC cookie processing. */
static inline bool AACCookieGetTag( uint8_t *p_tag, const uint8_t *p, uint64_t *p_offset, uint64_t i_size )
{
    if( *p_offset + 1 > i_size )
        return false;
    *p_tag = *( p + *p_offset );
    *p_offset += 1;
    return true;
}
