static int spectrometer_Run(visual_effect_t * p_effect, vlc_object_t *p_aout,
                            const block_t * p_buffer , picture_t * p_picture)
{
#define Y(R,G,B) ((uint8_t)( (R * .299) + (G * .587) + (B * .114) ))
#define U(R,G,B) ((uint8_t)( (R * -.169) + (G * -.332) + (B * .500) + 128 ))
#define V(R,G,B) ((uint8_t)( (R * .500) + (G * -.419) + (B * -.0813) + 128 ))
    float p_output[FFT_BUFFER_SIZE];  /* Raw FFT Result  */
    int *height;                      /* Bar heights */
    int *peaks;                       /* Peaks */
    int i_80_bands;                   /* number of bands : 80 if true else 20 */
    int i_nb_bands;                   /* number of bands : 80 or 20 */
    int i_band_width;                 /* width of bands */
    int i_separ;                      /* Should we let blanks ? */
    int i_amp;                        /* Vertical amplification */
    int i_peak;                       /* Should we draw peaks ? */
    int i_original;          /* original spectrum graphic routine */
    int i_rad;               /* radius of circle of base of bands */
    int i_sections;          /* sections of spectranalysis */
    int i_extra_width;       /* extra width on peak */
    int i_peak_height;       /* height of peak */
    int c;                   /* sentinel container of total spectral sections */
    double band_sep_angle;   /* angled separation between beginning of each band */
    double section_sep_angle;/* "   "    '     "    '    "     "    spectrum section */
    int max_band_length;     /* try not to go out of screen */
    int i_show_base;         /* Should we draw base of circle ? */
    int i_show_bands;        /* Should we draw bands ? */
    //int i_invert_bands;      /* do the bands point inward ? */
    double a;                /* for various misc angle situations in radians */
    int x,y,xx,yy;           /* various misc x/y */
    char color1;             /* V slide on a YUV color cube */
    //char color2;             /* U slide.. ?  color2 fade color ? */
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
    const double y_scale =  3.60673760222;  /* (log 256) */
    fft_state *p_state;                 /* internal FFT data */
    DEFINE_WIND_CONTEXT( wind_ctx );    /* internal window data */
    int i , j , k;
    int i_line = 0;
    int16_t p_dest[FFT_BUFFER_SIZE];      /* Adapted FFT result */
    int16_t p_buffer1[FFT_BUFFER_SIZE];   /* Buffer on which we perform
                                             the FFT (first channel) */
    float *p_buffl =                     /* Original buffer */
            (float*)p_buffer->p_buffer;
    int16_t  *p_buffs;                    /* int16_t converted buffer */
    int16_t  *p_s16_buff;                /* int16_t converted buffer */
    if (!p_buffer->i_nb_samples) {
        msg_Err(p_aout, "no samples yet");
        return -1;
    }
    /* Create the data struct if needed */
    spectrometer_data *p_data = p_effect->p_data;
    if( !p_data )
    {
        p_data = malloc( sizeof(spectrometer_data) );
        if( !p_data )
            return -1;
        p_data->peaks = calloc( 80, sizeof(int) );
        if( !p_data->peaks )
        {
            free( p_data );
            return -1;
        }
        p_data->i_prev_nb_samples = 0;
        p_data->p_prev_s16_buff = NULL;
        window_get_param( p_aout, &p_data->wind_param );
        p_effect->p_data = (void*)p_data;
    }
    peaks = p_data->peaks;
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
    i_original     = var_InheritInteger( p_aout, "spect-show-original" );
    i_80_bands     = var_InheritInteger( p_aout, "spect-80-bands" );
    i_separ        = var_InheritInteger( p_aout, "spect-separ" );
    i_amp          = var_InheritInteger( p_aout, "spect-amp" );
    i_peak         = var_InheritInteger( p_aout, "spect-show-peaks" );
    i_show_base    = var_InheritInteger( p_aout, "spect-show-base" );
    i_show_bands   = var_InheritInteger( p_aout, "spect-show-bands" );
    i_rad          = var_InheritInteger( p_aout, "spect-radius" );
    i_sections     = var_InheritInteger( p_aout, "spect-sections" );
    i_extra_width  = var_InheritInteger( p_aout, "spect-peak-width" );
    i_peak_height  = var_InheritInteger( p_aout, "spect-peak-height" );
    color1         = var_InheritInteger( p_aout, "spect-color" );
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
    if( !height)
        return -1;
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
        msg_Err(p_aout,"unable to initialize FFT transform");
        free( height );
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
    for ( i = 0 ; i < FFT_BUFFER_SIZE; i++)
    {
        p_output[i]    = 0;
        p_buffer1[i] = *p_buffs;
        p_buffs += p_effect->i_nb_chans;
        if( p_buffs >= &p_s16_buff[p_buffer->i_nb_samples * p_effect->i_nb_chans] )
            p_buffs = p_s16_buff;
    }
    window_scale_in_place( p_buffer1, &wind_ctx );
    fft_perform( p_buffer1, p_output, p_state);
    for(i = 0; i < FFT_BUFFER_SIZE; i++)
    {
        int sqrti = sqrt(p_output[i]);
        p_dest[i] = sqrti >> 8;
    }
    i_nb_bands *= i_sections;
    for ( i = 0 ; i< i_nb_bands/i_sections ;i++)
    {
        /* We search the maximum on one scale */
        for( j = xscale[i] , y=0 ; j< xscale[ i + 1 ] ; j++ )
        {
            if ( p_dest[j] > y )
                 y = p_dest[j];
        }
        /* Calculate the height of the bar */
        y >>=7;/* remove some noise */
        if( y != 0)
        {
            int logy = log(y);
            height[i] = logy * y_scale;
            if(height[i] > 150)
                height[i] = 150;
        }
        else
        {
            height[i] = 0 ;
        }
        /* Draw the bar now */
        i_band_width = floor( p_effect->i_width / (i_nb_bands/i_sections)) ;
        if( i_amp * height[i] > peaks[i])
        {
            peaks[i] = i_amp * height[i];
        }
        else if (peaks[i] > 0 )
        {
            peaks[i] -= PEAK_SPEED;
            if( peaks[i] < i_amp * height[i] )
            {
                peaks[i] = i_amp * height[i];
            }
            if( peaks[i] < 0 )
            {
                peaks[i] = 0;
            }
        }
        if( i_original != 0 )
        {
        if( peaks[i] > 0 && i_peak )
        {
            if( peaks[i] >= p_effect->i_height )
                peaks[i] = p_effect->i_height - 2;
            i_line = peaks[i];
            for( j = 0 ; j< i_band_width - i_separ; j++)
            {
               for( k = 0 ; k< 3 ; k ++)
               {
                   //* Draw the peak
                     *(p_picture->p[0].p_pixels +
                    (p_effect->i_height - i_line -1 -k ) *
                     p_picture->p[0].i_pitch + (i_band_width*i +j) )
                                    = 0xff;
                    *(p_picture->p[1].p_pixels +
                     ( ( p_effect->i_height - i_line ) / 2 -1 -k/2 ) *
                     p_picture->p[1].i_pitch +
                    ( ( i_band_width * i + j ) /2  ) )
                                    = 0x00;
                   if( 0x04 * (i_line + k ) - 0x0f > 0 )
                   {
                       if ( 0x04 * (i_line + k ) -0x0f < 0xff)
                           *(p_picture->p[2].p_pixels  +
                            ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                             p_picture->p[2].i_pitch +
                             ( ( i_band_width * i + j ) /2  ) )
                                    = ( 0x04 * ( i_line + k ) ) -0x0f ;
                       else
                           *(p_picture->p[2].p_pixels  +
                            ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                             p_picture->p[2].i_pitch +
                             ( ( i_band_width * i + j ) /2  ) )
                                    = 0xff;
                   }
                   else
                   {
                        *(p_picture->p[2].p_pixels  +
                         ( ( p_effect->i_height - i_line ) / 2 - 1 -k/2 ) *
                         p_picture->p[2].i_pitch +
                         ( ( i_band_width * i + j ) /2  ) )
                               = 0x10 ;
                   }
               }
            }
        }
        if(height[i] * i_amp > p_effect->i_height)
            height[i] = floor(p_effect->i_height / i_amp );
        for(i_line = 0 ; i_line < i_amp * height[i]; i_line ++ )
        {
            for( j = 0 ; j< i_band_width - i_separ ; j++)
            {
               *(p_picture->p[0].p_pixels +
                 (p_effect->i_height - i_line -1) *
                  p_picture->p[0].i_pitch + (i_band_width*i +j) ) = 0xff;
                *(p_picture->p[1].p_pixels +
                 ( ( p_effect->i_height - i_line ) / 2 -1) *
                 p_picture->p[1].i_pitch +
                 ( ( i_band_width * i + j ) /2  ) ) = 0x00;
               if( 0x04 * i_line - 0x0f > 0 )
               {
                    if( 0x04 * i_line - 0x0f < 0xff )
                         *(p_picture->p[2].p_pixels  +
                          ( ( p_effect->i_height - i_line ) / 2 - 1) *
                           p_picture->p[2].i_pitch +
                           ( ( i_band_width * i + j ) /2  ) ) =
                               ( 0x04 * i_line) -0x0f ;
                    else
                         *(p_picture->p[2].p_pixels  +
                          ( ( p_effect->i_height - i_line ) / 2 - 1) *
                           p_picture->p[2].i_pitch +
                           ( ( i_band_width * i + j ) /2  ) ) =
                                       0xff;
               }
               else
               {
                    *(p_picture->p[2].p_pixels  +
                     ( ( p_effect->i_height - i_line ) / 2 - 1) *
                     p_picture->p[2].i_pitch +
                     ( ( i_band_width * i + j ) /2  ) ) =
                            0x10 ;
               }
            }
        }
        }
    }
    band_sep_angle = 360.0 / i_nb_bands;
    section_sep_angle = 360.0 / i_sections;
    if( i_peak_height < 1 )
        i_peak_height = 1;
    max_band_length = p_effect->i_height / 2 - ( i_rad + i_peak_height + 1 );
    i_band_width = floor( 360 / i_nb_bands - i_separ );
    if( i_band_width < 1 )
        i_band_width = 1;
    for( c = 0 ; c < i_sections ; c++ )
    for( i = 0 ; i < (i_nb_bands / i_sections) ; i++ )
    {
        /* DO A PEAK */
        if( peaks[i] > 0 && i_peak )
        {
            if( peaks[i] >= p_effect->i_height )
                peaks[i] = p_effect->i_height - 2;
            i_line = peaks[i];
            /* circular line pattern(so color blend is more visible) */
            for( j = 0 ; j < i_peak_height ; j++ )
            {
                //x = p_picture->p[0].i_pitch / 2;
                x = p_effect->i_width / 2;
                y = p_effect->i_height / 2;
                xx = x;
                yy = y;
                for( k = 0 ; k < (i_band_width + i_extra_width) ; k++ )
                {
                    x = xx;
                    y = yy;
                    a = ( (i+1) * band_sep_angle + section_sep_angle * (c+1) + k )
                        * 3.141592 / 180.0;
                    x += (double)( cos(a) * (double)( i_line + j + i_rad ) );
                    y += (double)( -sin(a) * (double)( i_line + j + i_rad ) );
                    *(p_picture->p[0].p_pixels + x + y * p_picture->p[0].i_pitch
                    ) = 255;/* Y(R,G,B); */
                    x /= 2;
                    y /= 2;
                    *(p_picture->p[1].p_pixels + x + y * p_picture->p[1].i_pitch
                    ) = 0;/* U(R,G,B); */
                    if( 0x04 * (i_line + k ) - 0x0f > 0 )
                    {
                        if ( 0x04 * (i_line + k ) -0x0f < 0xff)
                            *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                            ) = ( 0x04 * ( i_line + k ) ) -(color1-1);/* -V(R,G,B); */
                        else
                            *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                            ) = 255;/* V(R,G,B); */
                    }
                    else
                    {
                        *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                        ) = color1;/* V(R,G,B); */
                    }
                }
            }
        }
        if( (height[i] * i_amp) > p_effect->i_height )
            height[i] = floor( p_effect->i_height / i_amp );
        /* DO BASE OF BAND (mostly makes a circle) */
        if( i_show_base != 0 )
        {
            //x = p_picture->p[0].i_pitch / 2;
            x = p_effect->i_width / 2;
            y = p_effect->i_height / 2;
            a =  ( (i+1) * band_sep_angle + section_sep_angle * (c+1) )
                * 3.141592 / 180.0;
            x += (double)( cos(a) * (double)i_rad );/* newb-forceful casting */
            y += (double)( -sin(a) * (double)i_rad );
            *(p_picture->p[0].p_pixels + x + y * p_picture->p[0].i_pitch
            ) = 255;/* Y(R,G,B); */
            x /= 2;
            y /= 2;
            *(p_picture->p[1].p_pixels + x + y * p_picture->p[1].i_pitch
            ) = 0;/* U(R,G,B); */
            if( 0x04 * i_line - 0x0f > 0 )
            {
                if( 0x04 * i_line -0x0f < 0xff)
                    *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                    ) = ( 0x04 * i_line) -(color1-1);/* -V(R,G,B); */
                else
                    *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                    ) = 255;/* V(R,G,B); */
            }
            else
            {
                *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                ) = color1;/* V(R,G,B); */
            }
        }
        /* DO A BAND */
        if( i_show_bands != 0 )
        for( j = 0 ; j < i_band_width ; j++ )
        {
            x = p_effect->i_width / 2;
            y = p_effect->i_height / 2;
            xx = x;
            yy = y;
            a = ( (i+1) * band_sep_angle + section_sep_angle * (c+1) + j )
                * 3.141592/180.0;
            for( k = (i_rad+1) ; k < max_band_length ; k++ )
            {
                if( (k-i_rad) > height[i] )
                    break;/* uhh.. */
                x = xx;
                y = yy;
                x += (double)( cos(a) * (double)k );/* newbed! */
                y += (double)( -sin(a) * (double)k );
                *(p_picture->p[0].p_pixels + x + y * p_picture->p[0].i_pitch
                ) = 255;
                x /= 2;
                y /= 2;
                *(p_picture->p[1].p_pixels + x + y * p_picture->p[1].i_pitch
                ) = 0;
                if( 0x04 * i_line - 0x0f > 0 )
                {
                    if ( 0x04 * i_line -0x0f < 0xff)
                        *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                        ) = ( 0x04 * i_line) -(color1-1);
                    else
                        *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                        ) = 255;
                }
                else
                {
                    *(p_picture->p[2].p_pixels + x + y * p_picture->p[2].i_pitch
                    ) = color1;
                }
            }
        }
    }
    window_close( &wind_ctx );
    fft_close( p_state );
    free( height );
    return 0;
}