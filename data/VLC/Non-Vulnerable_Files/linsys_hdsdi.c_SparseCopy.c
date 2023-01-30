#undef Y2
static void SparseCopy( int16_t *p_dest, const int16_t *p_src,
                        size_t i_nb_samples, size_t i_offset, size_t i_stride )
{
    for ( size_t i = 0; i < i_nb_samples; i++ )
    {
        p_dest[2 * i] = p_src[i_offset];
        p_dest[2 * i + 1] = p_src[i_offset + 1];
        i_offset += 2 * i_stride;
    }
}
