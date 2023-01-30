 *****************************************************************************/
static block_t *DoWork( filter_t *p_filter, block_t *p_in_buf )
{
    float *pf_sum;
    float *pf_gain;
    float f_average = 0;
    int i, i_chan;
    int i_samples = p_in_buf->i_nb_samples;
    int i_channels = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    float *p_out = (float*)p_in_buf->p_buffer;
    float *p_in =  (float*)p_in_buf->p_buffer;
    struct filter_sys_t *p_sys = p_filter->p_sys;
    pf_sum = calloc( i_channels, sizeof(float) );
    if( !pf_sum )
        goto out;
    pf_gain = malloc( sizeof(float) * i_channels );
    if( !pf_gain )
    {
        free( pf_sum );
        goto out;
    }
    /* Calculate the average power level on this buffer */
    for( i = 0 ; i < i_samples; i++ )
    {
        for( i_chan = 0; i_chan < i_channels; i_chan++ )
        {
            float f_sample = p_in[i_chan];
            pf_sum[i_chan] += f_sample * f_sample;
        }
        p_in += i_channels;
    }
    /* sum now contains for each channel the sigma(value²) */
    for( i_chan = 0; i_chan < i_channels; i_chan++ )
    {
        /* Shift our lastbuff */
        memmove( &p_sys->p_last[ i_chan * p_sys->i_nb],
                        &p_sys->p_last[i_chan * p_sys->i_nb + 1],
                 (p_sys->i_nb-1) * sizeof( float ) );
        /* Insert the new average : sqrt(sigma(value²)) */
        p_sys->p_last[ i_chan * p_sys->i_nb + p_sys->i_nb - 1] =
                sqrt( pf_sum[i_chan] );
        pf_sum[i_chan] = 0;
        /* Get the average power on the lastbuff */
        f_average = 0;
        for( i = 0; i < p_sys->i_nb ; i++)
        {
            f_average += p_sys->p_last[ i_chan * p_sys->i_nb + i ];
        }
        f_average = f_average / p_sys->i_nb;
        /* Seuil arbitraire */
        p_sys->f_max = var_GetFloat( p_filter->p_parent, "norm-max-level" );
        //fprintf(stderr,"Average %f, max %f\n", f_average, p_sys->f_max );
        if( f_average > p_sys->f_max )
        {
             pf_gain[i_chan] = f_average / p_sys->f_max;
        }
        else
        {
           pf_gain[i_chan] = 1;
        }
    }
    /* Apply gain */
    for( i = 0; i < i_samples; i++)
    {
        for( i_chan = 0; i_chan < i_channels; i_chan++ )
        {
            p_out[i_chan] /= pf_gain[i_chan];
        }
        p_out += i_channels;
    }
    free( pf_sum );
    free( pf_gain );
    return p_in_buf;
out:
    block_Release( p_in_buf );
    return NULL;
}
