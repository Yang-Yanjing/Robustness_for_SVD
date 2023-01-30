}
static void get_custom_from_packedyuv422( picture_t *p_inpic,
                                          picture_t *p_outpic,
                                          int *m )
{
    int i_y_offset, i_u_offset, i_v_offset;
    if( GetPackedYuvOffsets( p_inpic->format.i_chroma, &i_y_offset,
                         &i_u_offset, &i_v_offset ) != VLC_SUCCESS )
        return;
    uint8_t *yin = p_inpic->p->p_pixels + i_y_offset;
    uint8_t *uin = p_inpic->p->p_pixels + i_u_offset;
    uint8_t *vin = p_inpic->p->p_pixels + i_v_offset;
    uint8_t *yout = p_outpic->p->p_pixels + i_y_offset;
    uint8_t *uout = p_outpic->p->p_pixels + i_u_offset;
    uint8_t *vout = p_outpic->p->p_pixels + i_v_offset;
    const int i_in_pitch  = p_inpic->p->i_pitch;
    const int i_out_pitch = p_outpic->p->i_pitch;
    const int i_visible_pitch = p_inpic->p->i_visible_pitch;
    const int i_visible_lines = p_inpic->p->i_visible_lines;
    const uint8_t *yend = yin + i_visible_lines * i_in_pitch;
    while( yin < yend )
    {
        const uint8_t *ylend = yin + i_visible_pitch;
        while( yin < ylend )
        {
            *uout = vlc_uint8( (*yin * m[3] + (*uin-U) * m[4] + (*vin-V) * m[5])
                      / 65536 + U );
            uout += 4;
            *vout = vlc_uint8( (*yin * m[6] + (*uin-U) * m[7] + (*vin-V) * m[8])
                     / 65536 + V );
            vout += 4;
            *yout = vlc_uint8( (*yin * m[0] + (*uin-U) * m[1] + (*vin-V) * m[2])
                       / 65536 );
            yin  += 2;
            yout += 2;
            *yout = vlc_uint8( (*yin * m[0] + (*uin-U) * m[1] + (*vin-V) * m[2])
                       / 65536 );
            yin  += 2;
            yout += 2;
            uin  += 4;
            vin  += 4;
        }
        yin  += i_in_pitch  - i_visible_pitch;
        yout += i_out_pitch - i_visible_pitch;
        uin  += i_in_pitch  - i_visible_pitch;
        uout += i_out_pitch - i_visible_pitch;
        vin  += i_in_pitch  - i_visible_pitch;
        vout += i_out_pitch - i_visible_pitch;
    }
}
