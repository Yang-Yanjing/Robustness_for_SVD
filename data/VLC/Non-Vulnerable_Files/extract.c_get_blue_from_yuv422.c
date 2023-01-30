}
static void get_blue_from_yuv422( picture_t *p_inpic, picture_t *p_outpic,
                                 int yp, int up, int vp )
{
    uint8_t *y1in = p_inpic->p[yp].p_pixels;
    uint8_t *uin  = p_inpic->p[up].p_pixels;
    uint8_t *y1out = p_outpic->p[yp].p_pixels;
    uint8_t *uout  = p_outpic->p[up].p_pixels;
    uint8_t *vout  = p_outpic->p[vp].p_pixels;
    const int i_in_pitch  = p_inpic->p[yp].i_pitch;
    const int i_out_pitch = p_outpic->p[yp].i_pitch;
    const int i_visible_pitch = p_inpic->p[yp].i_visible_pitch;
    const int i_visible_lines = p_inpic->p[yp].i_visible_lines;
    const int i_uv_visible_pitch = p_inpic->p[up].i_visible_pitch;
    const uint8_t *yend = y1in + i_visible_lines * i_in_pitch;
    while( y1in < yend )
    {
        const uint8_t *y1end = y1in + i_visible_pitch;
        while( y1in < y1end )
        {
/*
7471    13239   0
32768   58065   0
-5329   -9443   0
*/
            *uout++ = vlc_uint8( (*y1in* 32768 + (*uin - U) * 58065 )
                      / 65536 + U );
            *vout++ = vlc_uint8( (*y1in * -5329 + (*uin - U) * -9443 )
                      / 65536 + V );
            *y1out++ = vlc_uint8( (*y1in++ * 7471 + (*uin - U) * 13239 )
                       / 65536 );
            *y1out++ = vlc_uint8( (*y1in++ * 7471 + (*uin++ - U) * 13239 )
                       / 65536 );
        }
        y1in  += i_in_pitch  - i_visible_pitch;
        y1out += i_out_pitch - i_visible_pitch;
        uin   += p_inpic->p[up].i_pitch - i_uv_visible_pitch;
        uout  += p_outpic->p[up].i_pitch - i_uv_visible_pitch;
        vout  += p_outpic->p[vp].i_pitch - i_uv_visible_pitch;
    }
}
