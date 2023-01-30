}
static void GetReference( int *refu, int *refv, int *reflength,
                          uint32_t i_color )
{
    int i_red   = ( i_color & 0xFF0000 ) >> 16;
    int i_green = ( i_color & 0x00FF00 ) >> 8;
    int i_blue  = ( i_color & 0x0000FF );
    int i_u = (int8_t)(( -38 * i_red - 74 * i_green + 112 * i_blue + 128) >> 8) + 128;
    int i_v = (int8_t)(( 112 * i_red - 94 * i_green -  18 * i_blue + 128) >> 8) + 128;
    *refu = i_u - 0x80;
    *refv = i_v - 0x80;
    *reflength = sqrt(*refu * *refu + *refv * *refv);
}
