 *****************************************************************************/
static block_t *DoWork( filter_t * p_filter, block_t * p_in_buf )
{
    filter_sys_t * p_sys = p_filter->p_sys;
    float * p_in = (float*) p_in_buf->p_buffer;
    size_t i_nb_samples = p_in_buf->i_nb_samples;
    size_t i_nb_channels = aout_FormatNbChannels( &p_filter->fmt_out.audio );
    size_t i_nb_rear = 0;
    size_t i;
    block_t *p_out_buf = block_Alloc(
                                sizeof(float) * i_nb_samples * i_nb_channels );
    if( !p_out_buf )
        goto out;
    float * p_out = (float*) p_out_buf->p_buffer;
    p_out_buf->i_nb_samples = i_nb_samples;
    p_out_buf->i_dts        = p_in_buf->i_dts;
    p_out_buf->i_pts        = p_in_buf->i_pts;
    p_out_buf->i_length     = p_in_buf->i_length;
    memset( p_out, 0, p_out_buf->i_buffer );
    if( p_sys->i_rear_left >= 0 )
    {
        ++i_nb_rear;
    }
    if( p_sys->i_rear_center >= 0 )
    {
        ++i_nb_rear;
    }
    if( p_sys->i_rear_right >= 0 )
    {
        ++i_nb_rear;
    }
    for( i = 0; i < i_nb_samples; ++i )
    {
        float f_left = p_in[ i * 2 ];
        float f_right = p_in[ i * 2 + 1 ];
        float f_rear = ( f_left - f_right ) / i_nb_rear;
        if( p_sys->i_center >= 0 )
        {
            float f_center = f_left + f_right;
            f_left -= f_center / 2;
            f_right -= f_center / 2;
            p_out[ i * i_nb_channels + p_sys->i_center ] = f_center;
        }
        if( p_sys->i_left >= 0 )
        {
            p_out[ i * i_nb_channels + p_sys->i_left ] = f_left;
        }
        if( p_sys->i_right >= 0 )
        {
            p_out[ i * i_nb_channels + p_sys->i_right ] = f_right;
        }
        if( p_sys->i_rear_left >= 0 )
        {
            p_out[ i * i_nb_channels + p_sys->i_rear_left ] = f_rear;
        }
        if( p_sys->i_rear_center >= 0 )
        {
            p_out[ i * i_nb_channels + p_sys->i_rear_center ] = f_rear;
        }
        if( p_sys->i_rear_right >= 0 )
        {
            p_out[ i * i_nb_channels + p_sys->i_rear_right ] = f_rear;
        }
    }
out:
    block_Release( p_in_buf );
    return p_out_buf;
}
