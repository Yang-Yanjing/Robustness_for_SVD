}
static inline bool AACCookieTagLen( uint64_t *p_tag_len, const uint8_t *p, uint64_t *p_offset, uint64_t i_size )
{
    uint32_t i_int_size;
    if( ParseVarLenInteger( p + *p_offset, i_size - *p_offset, p_tag_len, &i_int_size ))
        return false;
    *p_offset += i_int_size;
    return true;
}
