}
static int ReallocBuffer( block_t **pp_out_buf,
                          float **pp_out, size_t i_out,
                          int i_nb_channels, int i_bytes_per_frame )
{
    if( i_out < (*pp_out_buf)->i_buffer/i_bytes_per_frame )
        return VLC_SUCCESS;
    /* It may happen when the wing size changes */
    const unsigned i_extra_frame = 256;
    *pp_out_buf = block_Realloc( *pp_out_buf, 0,
                                 (*pp_out_buf)->i_buffer +
                                    i_extra_frame * i_bytes_per_frame );
    if( !*pp_out_buf )
        return VLC_EGENERIC;
    *pp_out = (float*)(*pp_out_buf)->p_buffer + i_out * i_nb_channels;
    memset( *pp_out, 0, i_extra_frame * i_bytes_per_frame );
    return VLC_SUCCESS;
}
