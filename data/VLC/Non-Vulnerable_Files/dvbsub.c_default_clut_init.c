 *****************************************************************************/
static void default_clut_init( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    uint8_t i;
#define RGB_TO_Y(r, g, b) ((int16_t) 77 * r + 150 * g + 29 * b) / 256;
#define RGB_TO_U(r, g, b) ((int16_t) -44 * r - 87 * g + 131 * b) / 256;
#define RGB_TO_V(r, g, b) ((int16_t) 131 * r - 110 * g - 21 * b) / 256;
    /* 4 entries CLUT */
    for( i = 0; i < 4; i++ )
    {
        uint8_t R = 0, G = 0, B = 0, T = 0;
        if( !(i & 0x2) && !(i & 0x1) ) T = 0xFF;
        else if( !(i & 0x2) && (i & 0x1) ) R = G = B = 0xFF;
        else if( (i & 0x2) && !(i & 0x1) ) R = G = B = 0;
        else R = G = B = 0x7F;
        p_sys->default_clut.c_2b[i].Y = RGB_TO_Y(R,G,B);
        p_sys->default_clut.c_2b[i].Cb = RGB_TO_V(R,G,B);
        p_sys->default_clut.c_2b[i].Cr = RGB_TO_U(R,G,B);
        p_sys->default_clut.c_2b[i].T = T;
    }
    /* 16 entries CLUT */
    for( i = 0; i < 16; i++ )
    {
        uint8_t R = 0, G = 0, B = 0, T = 0;
        if( !(i & 0x8) )
        {
            if( !(i & 0x4) && !(i & 0x2) && !(i & 0x1) )
            {
                T = 0xFF;
            }
            else
            {
                R = (i & 0x1) ? 0xFF : 0;
                G = (i & 0x2) ? 0xFF : 0;
                B = (i & 0x4) ? 0xFF : 0;
            }
        }
        else
        {
            R = (i & 0x1) ? 0x7F : 0;
            G = (i & 0x2) ? 0x7F : 0;
            B = (i & 0x4) ? 0x7F : 0;
        }
        p_sys->default_clut.c_4b[i].Y = RGB_TO_Y(R,G,B);
        p_sys->default_clut.c_4b[i].Cr = RGB_TO_V(R,G,B);
        p_sys->default_clut.c_4b[i].Cb = RGB_TO_U(R,G,B);
        p_sys->default_clut.c_4b[i].T = T;
    }
    /* 256 entries CLUT */
    memset( p_sys->default_clut.c_8b, 0xFF, 256 * sizeof(dvbsub_color_t) );
}
