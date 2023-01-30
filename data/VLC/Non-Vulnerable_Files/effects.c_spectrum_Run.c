} spectrum_data;
static int spectrum_Run(visual_effect_t * p_effect, vlc_object_t *p_aout,
                        const block_t * p_buffer , picture_t * p_picture)
{
    spectrum_data *p_data = p_effect->p_data;
    float p_output[FFT_BUFFER_SIZE];  /* Raw FFT Result  */
    int *height;                      /* Bar heights */
    int *peaks;                       /* Peaks */
    int *prev_heights;                /* Previous bar heights */
    int i_80_bands;                   /* number of bands : 80 if true else 20 */
    int i_nb_bands;                   /* number of bands : 80 or 20 */
    int i_band_width;                 /* width of bands */
    int i_start;                      /* first band horizontal position */
    int i_peak;                       /* Should we draw peaks ? */
    /* Horizontal scale for 20-band equalizer */
    const int xscale1[]={0,1,2,3,4,5,6,7,8,11,15,20,27,
                        36,47,62,82,107,141,184,255};
    /* Horizontal scale for 80-band equalizer */
    const int xscale2[] =
    {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
     19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,
     35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
     52,53,54,55,56,57,58,59,61,63,67,72,77,82,87,93,99,105,
     110,115,121,130,141,152,163,174,185,200,255};
    const int *xscale;
    fft_state *p_state;                 /* internal FFT data */
    DEFINE_WIND_CONTEXT( wind_ctx );    /* internal window data */
    int i , j , y , k;
    int i_line;
    int16_t p_dest[FFT_BUFFER_SIZE];      /* Adapted FFT result */
    int16_t p_buffer1[FFT_BUFFER_SIZE];   /* Buffer on which we perform
                                             the FFT (first channel) */
    float *p_buffl =                     /* Original buffer */
            (float*)p_buffer->p_buffer;
    int16_t  *p_buffs;                    /* int16_t converted buffer */
    int16_t  *p_s16_buff;                 /* int16_t converted buffer */
    if (!p_buffer->i_nb_samples) {
        msg_Err(p_aout, "no samples yet");
        return -1;
    }
    /* Create p_data if needed */
    if( !p_data )
    {
        p_effect->p_data = p_data = malloc( sizeof( spectrum_data ) );
        if( !p_data )
            return -1;
        p_data->peaks = calloc( 80, sizeof(int) );
        p_data->prev_heights = calloc( 80, sizeof(int) );
        p_data->i_prev_nb_samples = 0;
        p_data->p_prev_s16_buff = NULL;
        window_get_param( p_aout, &p_data->wind_param );
    }
    peaks = (int *)p_data->peaks;
    prev_heights = (int *)p_data->prev_heights;
    /* Allocate the buffer only if the number of samples change */
    if( p_buffer->i_nb_samples != p_data->i_prev_nb_samples )
    {
        free( p_data->p_prev_s16_buff );
        p_data->p_prev_s16_buff = malloc( p_buffer->i_nb_samples *
                                          p_effect->i_nb_chans *
                                          sizeof(int16_t));
        p_data->i_prev_nb_samples = p_buffer->i_nb_samples;
        if( !p_data->p_prev_s16_buff )
            return -1;
    }
    p_buffs = p_s16_buff = p_data->p_prev_s16_buff;
    i_80_bands = var_InheritInteger( p_aout, "visual-80-bands" );
    i_peak     = var_InheritInteger( p_aout, "visual-peaks" );
    if( i_80_bands != 0)
    {
        xscale = xscale2;
        i_nb_bands = 80;
    }
    else
    {
        xscale = xscale1;
        i_nb_bands = 20;
    }
    height = malloc( i_nb_bands * sizeof(int) );
    if( !height )
    {
        return -1;
    }
    /* Convert the buffer to int16_t  */
    /* Pasted from float32tos16.c */
    for (i = p_buffer->i_nb_samples * p_effect->i_nb_chans; i--; )
    {
        union { float f; int32_t i; } u;
        u.f = *p_buffl + 384.0;
        if(u.i >  0x43c07fff ) * p_buffs = 32767;
        else if ( u.i < 0x43bf8000 ) *p_buffs = -32768;
        else *p_buffs = u.i - 0x43c00000;
        p_buffl++ ; p_buffs++ ;
    }
    p_state  = visual_fft_init();
    if( !p_state)
    {
        free( height );
        msg_Err(p_aout,"unable to initialize FFT transform");
        return -1;
    }
    if( !window_init( FFT_BUFFER_SIZE, &p_data->wind_param, &wind_ctx ) )
    {
        fft_close( p_state );
        free( height );
        msg_Err(p_aout,"unable to initialize FFT window");
        return -1;
    }
    p_buffs = p_s16_buff;
    for ( i = 0 ; i < FFT_BUFFER_SIZE ; i++)
    {
        p_output[i]  = 0;
        p_buffer1[i] = *p_buffs;
        p_buffs += p_effect->i_nb_chans;
        if( p_buffs >= &p_s16_buff[p_buffer->i_nb_samples * p_effect->i_nb_chans] )
            p_buffs = p_s16_buff;
    }
    window_scale_in_place( p_buffer1, &wind_ctx );
    fft_perform( p_buffer1, p_output, p_state);
    for( i = 0; i< FFT_BUFFER_SIZE ; i++ )
        p_dest[i] = p_output[i] *  ( 2 ^ 16 ) / ( ( FFT_BUFFER_SIZE / 2 * 32768 ) ^ 2 );
    /* Compute the horizontal position of the first band */
    i_band_width = floor( p_effect->i_width / i_nb_bands);
    i_start = ( p_effect->i_width - i_band_width * i_nb_bands ) / 2;
    for ( i = 0 ; i < i_nb_bands ;i++)
    {
        /* We search the maximum on one scale */
        for( j = xscale[i], y = 0; j< xscale[ i + 1 ]; j++ )
        {
            if ( p_dest[j] > y )
                 y = p_dest[j];
        }
        /* Calculate the height of the bar */
        if( y != 0 )
        {
            height[i] = log( y ) * 30;
            if( height[i] > 380 )
                height[i] = 380;
        }
        else
            height[ i ] = 0;
        /* Draw the bar now */
        if( height[i] > peaks[i] )
        {
            peaks[i] = height[i];
        }
        else if( peaks[i] > 0 )
        {
            peaks[i] -= PEAK_SPEED;
            if( peaks[i] < height[i] )
            {
                peaks[i] = height[i];
            }
            if( peaks[i] < 0 )
            {
                peaks[i] = 0;
            }
        }
        /* Decrease the bars if needed */
        if( height[i] <= prev_heights[i] - BAR_DECREASE_SPEED )
        {
            height[i] = prev_heights[i];
            height[i] -= BAR_DECREASE_SPEED;
        }
        prev_heights[i] = height[i];
        if( peaks[i] > 0 && i_peak )
        {
            if( peaks[i] >= p_effect->i_height )
                peaks[i] = p_effect->i_height - 2;
            i_line = peaks[i];
            for( j = 0; j < i_band_width - 1; j++ )
            {
               for( k = 0; k < 3; k ++ )
               {
                   /* Draw the peak */
                   *(p_picture->p[0].p_pixels +
                    ( p_effect->i_height - i_line -1 -k ) *
                     p_picture->p[0].i_pitch +
                     ( i_start + i_band_width*i + j ) )
                                    = 0xff;
                   *(p_picture->p[1].p_pixels +
                    ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                     p_picture->p[1].i_pitch +
                     ( ( i_start + i_band_width * i + j ) /2  ) )
                                    = 0x00;
                   if( i_line + k - 0x0f > 0 )
                   {
                       if ( i_line + k - 0x0f < 0xff )
                           *(p_picture->p[2].p_pixels  +
                            ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                             p_picture->p[2].i_pitch +
                             ( ( i_start + i_band_width * i + j ) /2  ) )
                                    = ( i_line + k ) - 0x0f;
                       else
                           *(p_picture->p[2].p_pixels  +
                            ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                             p_picture->p[2].i_pitch +
                             ( ( i_start + i_band_width * i + j ) /2  ) )
                                    = 0xff;
                   }
                   else
                   {
                        *(p_picture->p[2].p_pixels  +
                         ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                         p_picture->p[2].i_pitch +
                         ( ( i_start + i_band_width * i + j ) /2  ) )
                               = 0x10 ;
                   }
               }
            }
        }
        if(height[i] > p_effect->i_height)
            height[i] = floor(p_effect->i_height );
        for( i_line = 0; i_line < height[i]; i_line++ )
        {
            for( j = 0 ; j < i_band_width - 1; j++)
            {
               *(p_picture->p[0].p_pixels +
                 (p_effect->i_height - i_line - 1) *
                  p_picture->p[0].i_pitch +
                  ( i_start + i_band_width*i + j ) ) = 0xff;
               *(p_picture->p[1].p_pixels +
                 ( ( p_effect->i_height - i_line ) / 2 - 1) *
                 p_picture->p[1].i_pitch +
                 ( ( i_start + i_band_width * i + j ) /2  ) ) = 0x00;
               if( i_line - 0x0f > 0 )
               {
                    if( i_line - 0x0f < 0xff )
                         *(p_picture->p[2].p_pixels  +
                           ( ( p_effect->i_height - i_line ) / 2 - 1) *
                           p_picture->p[2].i_pitch +
                           ( ( i_start + i_band_width * i + j ) /2  ) ) =
                               i_line - 0x0f;
                    else
                         *(p_picture->p[2].p_pixels  +
                           ( ( p_effect->i_height - i_line ) / 2  - 1) *
                           p_picture->p[2].i_pitch +
                           ( ( i_start + i_band_width * i + j ) /2  ) ) =
                                       0xff;
               }
               else
               {
                    *(p_picture->p[2].p_pixels  +
                      ( ( p_effect->i_height - i_line ) / 2  - 1) *
                      p_picture->p[2].i_pitch +
                      ( ( i_start + i_band_width * i + j ) /2  ) ) =
                            0x10;
               }
            }
        }
    }
    window_close( &wind_ctx );
    fft_close( p_state );
    free( height );
    return 0;
}
