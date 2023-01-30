}
static inline uint32_t bs_read_ue( bs_t *s )
{
    int i = 0;
    while( bs_read1( s ) == 0 && s->p < s->p_end && i < 32 )
    {
        i++;
    }
    return( ( 1u << i) - 1 + bs_read( s, i ) );
}
