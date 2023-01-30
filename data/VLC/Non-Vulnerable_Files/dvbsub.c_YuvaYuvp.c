 */
static subpicture_t *YuvaYuvp( subpicture_t *p_subpic )
{
    subpicture_region_t *p_region = NULL;
    if( !p_subpic ) return NULL;
    for( p_region = p_subpic->p_region; p_region; p_region = p_region->p_next )
    {
        video_format_t *p_fmt = &p_region->fmt;
        int i = 0, j = 0, n = 0, p = 0;
        int i_max_entries = 256;
#ifdef RANDOM_DITHERING
        int i_seed = 0xdeadbeef; /* random seed */
#else
        int *pi_delta;
#endif
        int i_pixels = p_region->p_picture->p[0].i_visible_lines
                        * p_region->p_picture->p[0].i_pitch;
        int i_iterator = p_region->p_picture->p[0].i_visible_lines * 3 / 4
                            * p_region->p_picture->p[0].i_pitch
                        + p_region->p_picture->p[0].i_pitch * 1 / 3;
        int i_tolerance = 0;
#ifdef DEBUG_DVBSUB1
        /* p_enc not valid here */
        msg_Dbg( p_enc, "YuvaYuvp: i_pixels=%d, i_iterator=%d", i_pixels, i_iterator );
#endif
        p_fmt->i_chroma = VLC_CODEC_YUVP;
        p_fmt->p_palette = (video_palette_t *) malloc( sizeof( video_palette_t ) );
        if( !p_fmt->p_palette ) break;
        p_fmt->p_palette->i_entries = 0;
        /* Find best iterator using Euclide’s algorithm */
        for( ; i_iterator > 1 ; i_iterator-- )
        {
            int a = i_pixels;
            int b = i_iterator;
            int c;
            while( b )
            {
                c = a % b;
                a = b;
                b = c;
            }
            if( a == 1 )
            {
                break;
            }
        }
        /* Count colors, build best palette */
        for( i_tolerance = 0; i_tolerance < 128; i_tolerance++ )
        {
            bool b_success = true;
            p_fmt->p_palette->i_entries = 0;
            for( i = 0; i < i_pixels ; )
            {
                uint8_t y, u, v, a;
                y = p_region->p_picture->p[0].p_pixels[i];
                u = p_region->p_picture->p[1].p_pixels[i];
                v = p_region->p_picture->p[2].p_pixels[i];
                a = p_region->p_picture->p[3].p_pixels[i];
                for( j = 0; j < p_fmt->p_palette->i_entries; j++ )
                {
                    if( abs((int)p_fmt->p_palette->palette[j][0] - (int)y) <= i_tolerance &&
                        abs((int)p_fmt->p_palette->palette[j][1] - (int)u) <= i_tolerance &&
                        abs((int)p_fmt->p_palette->palette[j][2] - (int)v) <= i_tolerance &&
                        abs((int)p_fmt->p_palette->palette[j][3] - (int)a) <= i_tolerance / 2 )
                    {
                        break;
                    }
                }
                if( j == p_fmt->p_palette->i_entries )
                {
                    p_fmt->p_palette->palette[j][0] = y;
                    p_fmt->p_palette->palette[j][1] = u;
                    p_fmt->p_palette->palette[j][2] = v;
                    p_fmt->p_palette->palette[j][3] = a;
                    p_fmt->p_palette->i_entries++;
                }
                if( p_fmt->p_palette->i_entries >= i_max_entries )
                {
                    b_success = false;
                    break;
                }
                i += i_iterator;
                if( i > i_pixels )
                {
                    i -= i_pixels;
                }
            }
            if( b_success )
            {
                break;
            }
        }
#ifdef DEBUG_DVBSUB1
        /* p_enc not valid here */
        msg_Dbg( p_enc, "best palette has %d colors", p_fmt->p_palette->i_entries );
#endif
#ifndef RANDOM_DITHERING
        pi_delta = xmalloc( ( p_region->p_picture->p[0].i_pitch + 1 )
                            * sizeof(int) * 4  );
        for( i = 0; i < (p_region->p_picture->p[0].i_pitch + 1) * 4 ; i++ )
        {
            pi_delta[ i ] = 0;
        }
#endif
        /* Fill image with our new colours */
        for( p = 0; p < p_region->p_picture->p[0].i_visible_lines ; p++ )
        {
            int i_ydelta = 0, i_udelta = 0, i_vdelta = 0, i_adelta = 0;
            for( n = 0; n < p_region->p_picture->p[0].i_pitch ; n++ )
            {
                int i_offset = p * p_region->p_picture->p[0].i_pitch + n;
                int y, u, v, a;
                int i_mindist, i_best;
                y = (int)p_region->p_picture->p[0].p_pixels[i_offset];
                u = (int)p_region->p_picture->p[1].p_pixels[i_offset];
                v = (int)p_region->p_picture->p[2].p_pixels[i_offset];
                a = (int)p_region->p_picture->p[3].p_pixels[i_offset];
                /* Add dithering compensation */
#ifdef RANDOM_DITHERING
                y += ((i_seed & 0xff) - 0x80) * i_tolerance / 0x80;
                u += (((i_seed >> 8) & 0xff) - 0x80) * i_tolerance / 0x80;
                v += (((i_seed >> 16) & 0xff) - 0x80) * i_tolerance / 0x80;
                a += (((i_seed >> 24) & 0xff) - 0x80) * i_tolerance / 0x80;
#else
                y += i_ydelta + pi_delta[ n * 4 ];
                u += i_udelta + pi_delta[ n * 4 + 1 ];
                v += i_vdelta + pi_delta[ n * 4 + 2 ];
                a += i_adelta + pi_delta[ n * 4 + 3 ];
#endif
                /* Find best colour in palette */
                for( i_mindist = 99999999, i_best = 0, j = 0; j < p_fmt->p_palette->i_entries; j++ )
                {
                    int i_dist = 0;
                    i_dist += abs((int)p_fmt->p_palette->palette[j][0] - y);
                    i_dist += abs((int)p_fmt->p_palette->palette[j][1] - u);
                    i_dist += abs((int)p_fmt->p_palette->palette[j][2] - v);
                    i_dist += 2 * abs((int)p_fmt->p_palette->palette[j][3] - a);
                    if( i_dist < i_mindist )
                    {
                        i_mindist = i_dist;
                        i_best = j;
                    }
                }
                /* Set pixel to best color */
                p_region->p_picture->p[0].p_pixels[i_offset] = i_best;
                /* Update dithering state */
#ifdef RANDOM_DITHERING
                i_seed = (i_seed * 0x1283837) ^ 0x789479 ^ (i_seed >> 13);
#else
                i_ydelta = y - (int)p_fmt->p_palette->palette[i_best][0];
                i_udelta = u - (int)p_fmt->p_palette->palette[i_best][1];
                i_vdelta = v - (int)p_fmt->p_palette->palette[i_best][2];
                i_adelta = a - (int)p_fmt->p_palette->palette[i_best][3];
                pi_delta[ n * 4 ] = i_ydelta * 3 / 8;
                pi_delta[ n * 4 + 1 ] = i_udelta * 3 / 8;
                pi_delta[ n * 4 + 2 ] = i_vdelta * 3 / 8;
                pi_delta[ n * 4 + 3 ] = i_adelta * 3 / 8;
                i_ydelta = i_ydelta * 5 / 8;
                i_udelta = i_udelta * 5 / 8;
                i_vdelta = i_vdelta * 5 / 8;
                i_adelta = i_adelta * 5 / 8;
#endif
            }
        }
#ifndef RANDOM_DITHERING
        free( pi_delta );
#endif
        /* pad palette */
        for( i = p_fmt->p_palette->i_entries; i < i_max_entries; i++ )
        {
            p_fmt->p_palette->palette[i][0] = 0;
            p_fmt->p_palette->palette[i][1] = 0;
            p_fmt->p_palette->palette[i][2] = 0;
            p_fmt->p_palette->palette[i][3] = 0;
        }
        p_fmt->p_palette->i_entries = i_max_entries;
#ifdef DEBUG_DVBSUB1
        /* p_enc not valid here */
        msg_Dbg( p_enc, "best palette has %d colors", p_fmt->p_palette->i_entries );
#endif
    }
    return p_subpic;
} /* End of hack */
