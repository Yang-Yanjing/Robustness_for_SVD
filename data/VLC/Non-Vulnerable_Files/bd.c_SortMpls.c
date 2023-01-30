}
static int SortMpls( const void *a, const void *b )
{
    const bd_mpls_t * const *pp_mpls_a = a;
    const bd_mpls_t * const *pp_mpls_b = b;
    const int64_t i_length_a = GetMplsUniqueDuration( *pp_mpls_a );
    const int64_t i_length_b = GetMplsUniqueDuration( *pp_mpls_b );
    if( i_length_a == i_length_b )
        return 0;
    return i_length_a < i_length_b ? 1 : -1;
}
