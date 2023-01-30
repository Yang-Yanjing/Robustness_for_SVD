}
static void get_green_from_yuv422( picture_t *p_inpic, picture_t *p_outpic,
                                   int yp, int up, int vp )
{
    uint8_t *y1in = p_inpic->p[yp].p_pixels;
    uint8_t *uin  = p_inpic->p[up].p_pixels;
    uint8_t *vin  = p_inpic->p[vp].p_pixels;
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
38470   -13239  -27473
-21710  7471    15504
-27439  9443    19595
*/
            *uout++ = vlc_uint8( (*y1in * -21710 + (*uin-U) * 7471 + (*vin-V) * 15504)
                      / 65536 + U );
            *vout++ = vlc_uint8( (*y1in * -27439 + (*uin-U) * 9443 + (*vin-V) * 19595)
                      / 65536 + V );
            *y1out++ = vlc_uint8( (*y1in++ * 38470 + (*uin-U) * -13239 + (*vin-V) * -27473)
                       / 65536 );
            *y1out++ = vlc_uint8( (*y1in++ * 38470 + (*uin++-U) * -13239 + (*vin++-V) * -27473)
                       / 65536 );
        }
        y1in  += i_in_pitch  - i_visible_pitch;
        y1out += i_out_pitch - i_visible_pitch;
        uin   += p_inpic->p[up].i_pitch  - i_uv_visible_pitch;
        uout  += p_outpic->p[up].i_pitch - i_uv_visible_pitch;
        vin   += p_inpic->p[vp].i_pitch  - i_uv_visible_pitch;
        vout  += p_outpic->p[vp].i_pitch - i_uv_visible_pitch;
    }
}
