}
static void ResampleFloat( filter_t *p_filter,
                           block_t **pp_out_buf,  size_t *pi_out,
                           float **pp_in,
                           int i_in, int i_in_end,
                           double d_factor, bool b_factor_old,
                           int i_nb_channels, int i_bytes_per_frame )
{
    filter_sys_t *p_sys = p_filter->p_sys;
    float *p_in = *pp_in;
    size_t i_out = *pi_out;
    float *p_out = (float*)(*pp_out_buf)->p_buffer + i_out * i_nb_channels;
    for( ; i_in < i_in_end; i_in++ )
    {
        if( b_factor_old && d_factor == 1 )
        {
            if( ReallocBuffer( pp_out_buf, &p_out,
                               i_out, i_nb_channels, i_bytes_per_frame ) )
                return;
            /* Just copy the samples */
            memcpy( p_out, p_in, i_bytes_per_frame );
            p_in += i_nb_channels;
            p_out += i_nb_channels;
            i_out++;
            continue;
        }
        while( p_sys->i_remainder < p_filter->fmt_out.audio.i_rate )
        {
            if( ReallocBuffer( pp_out_buf, &p_out,
                               i_out, i_nb_channels, i_bytes_per_frame ) )
                return;
            if( d_factor >= 1 )
            {
                /* FilterFloatUP() is faster if we can use it */
                /* Perform left-wing inner product */
                FilterFloatUP( SMALL_FILTER_FLOAT_IMP, SMALL_FILTER_FLOAT_IMPD,
                               SMALL_FILTER_NWING, p_in, p_out,
                               p_sys->i_remainder,
                               p_filter->fmt_out.audio.i_rate,
                               -1, i_nb_channels );
                /* Perform right-wing inner product */
                FilterFloatUP( SMALL_FILTER_FLOAT_IMP, SMALL_FILTER_FLOAT_IMPD,
                               SMALL_FILTER_NWING, p_in + i_nb_channels, p_out,
                               p_filter->fmt_out.audio.i_rate -
                               p_sys->i_remainder,
                               p_filter->fmt_out.audio.i_rate,
                               1, i_nb_channels );
#if 0
                /* Normalize for unity filter gain */
                for( i = 0; i < i_nb_channels; i++ )
                {
                    *(p_out+i) *= d_old_scale_factor;
                }
#endif
            }
            else
            {
                /* Perform left-wing inner product */
                FilterFloatUD( SMALL_FILTER_FLOAT_IMP, SMALL_FILTER_FLOAT_IMPD,
                               SMALL_FILTER_NWING, p_in, p_out,
                               p_sys->i_remainder,
                               p_filter->fmt_out.audio.i_rate, p_filter->fmt_in.audio.i_rate,
                               -1, i_nb_channels );
                /* Perform right-wing inner product */
                FilterFloatUD( SMALL_FILTER_FLOAT_IMP, SMALL_FILTER_FLOAT_IMPD,
                               SMALL_FILTER_NWING, p_in + i_nb_channels, p_out,
                               p_filter->fmt_out.audio.i_rate -
                               p_sys->i_remainder,
                               p_filter->fmt_out.audio.i_rate, p_filter->fmt_in.audio.i_rate,
                               1, i_nb_channels );
            }
            p_out += i_nb_channels;
            i_out++;
            p_sys->i_remainder += p_filter->fmt_in.audio.i_rate;
        }
        p_in += i_nb_channels;
        p_sys->i_remainder -= p_filter->fmt_out.audio.i_rate;
    }
    *pp_in  = p_in;
    *pi_out = i_out;
}
