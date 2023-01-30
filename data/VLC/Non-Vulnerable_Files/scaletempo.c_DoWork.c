 *****************************************************************************/
static block_t *DoWork( filter_t * p_filter, block_t * p_in_buf )
{
    filter_sys_t *p = p_filter->p_sys;
    if( p_filter->fmt_in.audio.i_rate == p->sample_rate )
        return p_in_buf;
    double scale = p_filter->fmt_in.audio.i_rate / (double)p->sample_rate;
    if( scale != p->scale ) {
      p->scale = scale;
      p->bytes_stride_scaled  = p->bytes_stride * p->scale;
      p->frames_stride_scaled = p->bytes_stride_scaled / p->bytes_per_frame;
      p->bytes_to_slide = 0;
      msg_Dbg( p_filter, "%.3f scale, %.3f stride_in, %i stride_out",
               p->scale,
               p->frames_stride_scaled,
               (int)( p->bytes_stride / p->bytes_per_frame ) );
    }
    size_t i_outsize = calculate_output_buffer_size ( p_filter, p_in_buf->i_buffer );
    block_t *p_out_buf = block_Alloc( i_outsize );
    if( p_out_buf == NULL )
        return NULL;
    size_t bytes_out = transform_buffer( p_filter,
        p_in_buf->p_buffer, p_in_buf->i_buffer,
        p_out_buf->p_buffer );
    p_out_buf->i_buffer     = bytes_out;
    p_out_buf->i_nb_samples = bytes_out / p->bytes_per_frame;
    p_out_buf->i_dts        = p_in_buf->i_dts;
    p_out_buf->i_pts        = p_in_buf->i_pts;
    p_out_buf->i_length     = p_in_buf->i_length;
    block_Release( p_in_buf );
    return p_out_buf;
}
