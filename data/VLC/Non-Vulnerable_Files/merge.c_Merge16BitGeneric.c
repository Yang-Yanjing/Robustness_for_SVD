}
void Merge16BitGeneric( void *_p_dest, const void *_p_s1,
                        const void *_p_s2, size_t i_bytes )
{
    uint16_t *p_dest = _p_dest;
    const uint16_t *p_s1 = _p_s1;
    const uint16_t *p_s2 = _p_s2;
    for( size_t i_words = i_bytes / 2; i_words > 0; i_words-- )
        *p_dest++ = ( *p_s1++ + *p_s2++ ) >> 1;
}
