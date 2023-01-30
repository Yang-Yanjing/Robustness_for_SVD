}
static block_t *Encode( encoder_t *enc, block_t *in )
{
    if( in == NULL )
        return NULL;
    block_t *out = block_Alloc( in->i_nb_samples
                                * enc->fmt_out.audio.i_bytes_per_frame );
    if( unlikely(out == NULL) )
        return NULL;
    out->i_flags      = in->i_flags;
    out->i_nb_samples = in->i_nb_samples;
    out->i_dts        = in->i_dts;
    out->i_pts        = in->i_pts;
    out->i_length     = in->i_length;
    void (*encode)(void *, const uint8_t *, unsigned) = (void *)enc->p_sys;
    if( encode != NULL )
        encode( out->p_buffer, in->p_buffer, in->i_nb_samples
                                             * enc->fmt_out.audio.i_channels );
    else {
        assert( out->i_buffer >= in->i_buffer );
        memcpy( out->p_buffer, in->p_buffer, in->i_buffer );
    }
    return out;
}
