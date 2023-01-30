}
static void combine_side_by_side_yuv420(picture_t *p_inpic, picture_t *p_outpic,
                                        int left, int right)
{
    uint8_t *y1inl = p_inpic->p[Y_PLANE].p_pixels;
    uint8_t *y2inl;
    uint8_t *uinl = p_inpic->p[U_PLANE].p_pixels;
    uint8_t *vinl = p_inpic->p[V_PLANE].p_pixels;
    uint8_t *y1out = p_outpic->p[Y_PLANE].p_pixels;
    uint8_t *y2out;
    uint8_t *uout = p_outpic->p[U_PLANE].p_pixels;
    uint8_t *vout = p_outpic->p[V_PLANE].p_pixels;
    const int in_pitch = p_inpic->p[Y_PLANE].i_pitch;
    const int out_pitch = p_outpic->p[Y_PLANE].i_pitch;
    const int visible_pitch = p_inpic->p[Y_PLANE].i_visible_pitch;
    const int visible_lines = p_inpic->p[Y_PLANE].i_visible_lines;
    const int uv_visible_pitch = p_inpic->p[U_PLANE].i_visible_pitch;
    const uint8_t *yend = y1inl + visible_lines * in_pitch;
    while (y1inl < yend)
    {
        uint8_t *y1inr = y1inl + visible_pitch/2;
        uint8_t *y2inr;
        uint8_t *uinr = uinl + uv_visible_pitch/2;
        uint8_t *vinr = vinl + uv_visible_pitch/2;
        const uint8_t *y1end = y1inr;
        y2inl = y1inl + in_pitch;
        y2inr = y1inr + in_pitch;
        y2out = y1out + out_pitch;
        while (y1inl < y1end)
        {
            int rl, gl, bl, rr, gr, br, r, g, b;
            int rshift = !!((0xff0000&left) && (0xff0000&right));
            int gshift = !!((0x00ff00&left) && (0x00ff00&right));
            int bshift = !!((0x0000ff&left) && (0x0000ff&right));
            yuv_to_rgb(&rl, &gl, &bl, *y1inl, *uinl, *vinl);
            yuv_to_rgb(&rr, &gr, &br, *y1inr, *uinr, *vinr);
            r = ((!!(0xff0000&left))*rl + (!!(0xff0000&right))*rr)>>rshift;
            g = ((!!(0x00ff00&left))*gl + (!!(0x00ff00&right))*gr)>>gshift;
            b = ((!!(0x0000ff&left))*bl + (!!(0x0000ff&right))*br)>>bshift;
            rgb_to_yuv(y1out, uout++, vout++, r, g, b);
            y1out[1] = *y1out;
            y1out+=2;
            y1inl++;
            y1inr++;
            yuv_to_rgb(&rl, &gl, &bl, *y1inl, *uinl, *vinl);
            yuv_to_rgb(&rr, &gr, &br, *y1inr, *uinr, *vinr);
            r = ((!!(0xff0000&left))*rl + (!!(0xff0000&right))*rr)>>rshift;
            g = ((!!(0x00ff00&left))*gl + (!!(0x00ff00&right))*gr)>>gshift;
            b = ((!!(0x0000ff&left))*bl + (!!(0x0000ff&right))*br)>>bshift;
            rgb_to_yuv(y1out, uout++, vout++, r, g, b);
            y1out[1] = *y1out;
            y1out+=2;
            y1inl++;
            y1inr++;
            yuv_to_rgb(&rl, &gl, &bl, *y2inl, *uinl, *vinl);
            yuv_to_rgb(&rr, &gr, &br, *y2inr, *uinr, *vinr);
            r = ((!!(0xff0000&left))*rl + (!!(0xff0000&right))*rr)>>rshift;
            g = ((!!(0x00ff00&left))*gl + (!!(0x00ff00&right))*gr)>>gshift;
            b = ((!!(0x0000ff&left))*bl + (!!(0x0000ff&right))*br)>>bshift;
            rgb_to_yuv(y2out, uout/*will be overwritten later, as will vout*/, vout, r, g, b);
            y2out[1] = *y2out;
            y2out+=2;
            y2inl++;
            y2inr++;
            yuv_to_rgb(&rl, &gl, &bl, *y2inl, *uinl, *vinl);
            yuv_to_rgb(&rr, &gr, &br, *y2inr, *uinr, *vinr);
            r = ((!!(0xff0000&left))*rl + (!!(0xff0000&right))*rr)>>rshift;
            g = ((!!(0x00ff00&left))*gl + (!!(0x00ff00&right))*gr)>>gshift;
            b = ((!!(0x0000ff&left))*bl + (!!(0x0000ff&right))*br)>>bshift;
            rgb_to_yuv(y2out, uout/*will be overwritten later, as will vout*/, vout, r, g, b);
            y2out[1] = *y2out;
            y2out+=2;
            y2inl++;
            y2inr++;
            uinl++;
            vinl++;
            uinr++;
            vinr++;
        }
        y1inl = y1inr + 2*in_pitch - visible_pitch;
        y1out += 2*out_pitch - visible_pitch;
        uinl = uinr + p_inpic->p[U_PLANE].i_pitch - uv_visible_pitch;
        vinl = vinr + p_inpic->p[V_PLANE].i_pitch - uv_visible_pitch;
        uout += p_outpic->p[U_PLANE].i_pitch - uv_visible_pitch;
        vout += p_outpic->p[V_PLANE].i_pitch - uv_visible_pitch;
    }
}
