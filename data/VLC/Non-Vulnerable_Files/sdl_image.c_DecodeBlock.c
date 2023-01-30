 ****************************************************************************/
static picture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    picture_t *p_pic = NULL;
    SDL_Surface *p_surface;
    SDL_RWops *p_rw;
    if( pp_block == NULL || *pp_block == NULL ) return NULL;
    p_block = *pp_block;
    if( p_block->i_flags & BLOCK_FLAG_DISCONTINUITY )
    {
        block_Release( p_block ); *pp_block = NULL;
        return NULL;
    }
    p_rw = SDL_RWFromConstMem( p_block->p_buffer, p_block->i_buffer );
    /* Decode picture. */
    p_surface = IMG_LoadTyped_RW( p_rw, 1, (char*)p_sys->psz_sdl_type );
    if ( p_surface == NULL )
    {
        msg_Warn( p_dec, "SDL_image couldn't load the image (%s)",
                  IMG_GetError() );
        goto error;
    }
    switch ( p_surface->format->BitsPerPixel )
    {
    case 16:
        p_dec->fmt_out.i_codec = VLC_CODEC_RGB16;
        break;
    case 8:
    case 24:
        p_dec->fmt_out.i_codec = VLC_CODEC_RGB24;
        break;
    case 32:
        p_dec->fmt_out.i_codec = VLC_CODEC_RGB32;
        break;
    default:
        msg_Warn( p_dec, "unknown bits/pixel format (%d)",
                  p_surface->format->BitsPerPixel );
        goto error;
    }
    p_dec->fmt_out.video.i_width = p_surface->w;
    p_dec->fmt_out.video.i_height = p_surface->h;
    p_dec->fmt_out.video.i_sar_num = 1;
    p_dec->fmt_out.video.i_sar_den = 1;
    /* Get a new picture. */
    p_pic = decoder_NewPicture( p_dec );
    if ( p_pic == NULL ) goto error;
    switch ( p_surface->format->BitsPerPixel )
    {
        case 8:
        {
            int i, j;
            uint8_t *p_src, *p_dst;
            uint8_t r, g, b;
            for ( i = 0; i < p_surface->h; i++ )
            {
                p_src = (uint8_t*)p_surface->pixels + i * p_surface->pitch;
                p_dst = p_pic->p[0].p_pixels + i * p_pic->p[0].i_pitch;
                for ( j = 0; j < p_surface->w; j++ )
                {
                    SDL_GetRGB( *(p_src++), p_surface->format,
                                &r, &g, &b );
                    *(p_dst++) = r;
                    *(p_dst++) = g;
                    *(p_dst++) = b;
                }
            }
            break;
        }
        case 16:
        {
            int i;
            uint8_t *p_src = p_surface->pixels;
            uint8_t *p_dst = p_pic->p[0].p_pixels;
            int i_pitch = p_pic->p[0].i_pitch < p_surface->pitch ?
                p_pic->p[0].i_pitch : p_surface->pitch;
            for ( i = 0; i < p_surface->h; i++ )
            {
                memcpy( p_dst, p_src, i_pitch );
                p_src += p_surface->pitch;
                p_dst += p_pic->p[0].i_pitch;
            }
            break;
        }
        case 24:
        {
            int i, j;
            uint8_t *p_src, *p_dst;
            uint8_t r, g, b;
            for ( i = 0; i < p_surface->h; i++ )
            {
                p_src = (uint8_t*)p_surface->pixels + i * p_surface->pitch;
                p_dst = p_pic->p[0].p_pixels + i * p_pic->p[0].i_pitch;
                for ( j = 0; j < p_surface->w; j++ )
                {
                    SDL_GetRGB( *(uint32_t*)p_src, p_surface->format,
                                &r, &g, &b );
                    *(p_dst++) = r;
                    *(p_dst++) = g;
                    *(p_dst++) = b;
                    p_src += 3;
                }
            }
            break;
        }
        case 32:
        {
            int i, j;
            uint8_t *p_src, *p_dst;
            uint8_t r, g, b, a;
            for ( i = 0; i < p_surface->h; i++ )
            {
                p_src = (uint8_t*)p_surface->pixels + i * p_surface->pitch;
                p_dst = p_pic->p[0].p_pixels + i * p_pic->p[0].i_pitch;
                for ( j = 0; j < p_surface->w; j++ )
                {
                    SDL_GetRGBA( *(uint32_t*)p_src, p_surface->format,
                                &r, &g, &b, &a );
                    *(p_dst++) = b;
                    *(p_dst++) = g;
                    *(p_dst++) = r;
                    *(p_dst++) = a;
                    p_src += 4;
                }
            }
            break;
        }
    }
    p_pic->date = (p_block->i_pts > VLC_TS_INVALID) ?
        p_block->i_pts : p_block->i_dts;
    SDL_FreeSurface( p_surface );
    block_Release( p_block ); *pp_block = NULL;
    return p_pic;
error:
    if ( p_surface != NULL ) SDL_FreeSurface( p_surface );
    block_Release( p_block ); *pp_block = NULL;
    return NULL;
}
