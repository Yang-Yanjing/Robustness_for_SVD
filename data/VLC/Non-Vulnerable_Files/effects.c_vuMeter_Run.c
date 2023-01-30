 *****************************************************************************/
static int vuMeter_Run(visual_effect_t * p_effect, vlc_object_t *p_aout,
                       const block_t * p_buffer , picture_t * p_picture)
{
    VLC_UNUSED(p_aout);
    float i_value_l = 0;
    float i_value_r = 0;
    /* Compute the peak values */
    for ( unsigned i = 0 ; i < p_buffer->i_nb_samples; i++ )
    {
        const float *p_sample = (float *)p_buffer->p_buffer;
        float ch;
        ch = p_sample[p_effect->i_idx_left] * 256;
        if (ch > i_value_l)
            i_value_l = ch;
        ch = p_sample[p_effect->i_idx_right] * 256;
        if (ch > i_value_r)
            i_value_r = ch;
        p_sample += p_effect->i_nb_chans;
    }
    i_value_l = fabsf(i_value_l);
    i_value_r = fabsf(i_value_r);
    /* Stay under maximum value admited */
    if ( i_value_l > 200 * M_PI_2 )
        i_value_l = 200 * M_PI_2;
    if ( i_value_r > 200 * M_PI_2 )
        i_value_r = 200 * M_PI_2;
    float *i_value;
    if( !p_effect->p_data )
    {
        /* Allocate memory to save hand positions */
        p_effect->p_data = malloc( 2 * sizeof(float) );
        i_value = p_effect->p_data;
        i_value[0] = i_value_l;
        i_value[1] = i_value_r;
    }
    else
    {
        /* Make the hands go down slowly if the current values are slower
           than the previous */
        i_value = p_effect->p_data;
        if ( i_value_l > i_value[0] - 6 )
            i_value[0] = i_value_l;
        else
            i_value[0] = i_value[0] - 6;
        if ( i_value_r > i_value[1] - 6 )
            i_value[1] = i_value_r;
        else
            i_value[1] = i_value[1] - 6;
    }
    int x, y;
    float teta;
    float teta_grad;
    int start_x = p_effect->i_width / 2 - 120; /* i_width.min = 532 (visual.c) */
    for ( int j = 0; j < 2; j++ )
    {
        /* Draw the two scales */
        int k = 0;
        teta_grad = GRAD_ANGLE_MIN;
        for ( teta = -M_PI_4; teta <= M_PI_4; teta = teta + 0.003 )
        {
            for ( unsigned i = 140; i <= 150; i++ )
            {
                y = i * cos(teta) + 20;
                x = i * sin(teta) + start_x + 240 * j;
                /* Compute the last color for the gradation */
                if (teta >= teta_grad + GRAD_INCR && teta_grad <= GRAD_ANGLE_MAX)
                {
                    teta_grad = teta_grad + GRAD_INCR;
                    k = k + 5;
                }
                *(p_picture->p[0].p_pixels +
                        (p_picture->p[0].i_lines - y - 1 ) * p_picture->p[0].i_pitch
                        + x ) = 0x45;
                *(p_picture->p[1].p_pixels +
                        (p_picture->p[1].i_lines - y / 2 - 1 ) * p_picture->p[1].i_pitch
                        + x / 2 ) = 0x0;
                *(p_picture->p[2].p_pixels +
                        (p_picture->p[2].i_lines - y / 2 - 1 ) * p_picture->p[2].i_pitch
                        + x / 2 ) = 0x4D + k;
            }
        }
        /* Draw the two hands */
        teta = (float)i_value[j] / 200 - M_PI_4;
        for ( int i = 0; i <= 150; i++ )
        {
            y = i * cos(teta) + 20;
            x = i * sin(teta) + start_x + 240 * j;
            *(p_picture->p[0].p_pixels +
                    (p_picture->p[0].i_lines - y - 1 ) * p_picture->p[0].i_pitch
                    + x ) = 0xAD;
            *(p_picture->p[1].p_pixels +
                    (p_picture->p[1].i_lines - y / 2 - 1 ) * p_picture->p[1].i_pitch
                    + x / 2 ) = 0xFC;
            *(p_picture->p[2].p_pixels +
                    (p_picture->p[2].i_lines - y / 2 - 1 ) * p_picture->p[2].i_pitch
                    + x / 2 ) = 0xAC;
        }
        /* Draw the hand bases */
        for ( teta = -M_PI_2; teta <= M_PI_2 + 0.01; teta = teta + 0.003 )
        {
            for ( int i = 0; i < 10; i++ )
            {
                y = i * cos(teta) + 20;
                x = i * sin(teta) + start_x + 240 * j;
                *(p_picture->p[0].p_pixels +
                        (p_picture->p[0].i_lines - y - 1 ) * p_picture->p[0].i_pitch
                        + x ) = 0xFF;
                *(p_picture->p[1].p_pixels +
                        (p_picture->p[1].i_lines - y / 2 - 1 ) * p_picture->p[1].i_pitch
                        + x / 2 ) = 0x80;
                *(p_picture->p[2].p_pixels +
                        (p_picture->p[2].i_lines - y / 2 - 1 ) * p_picture->p[2].i_pitch
                        + x / 2 ) = 0x80;
            }
        }
    }
    return 0;
}
