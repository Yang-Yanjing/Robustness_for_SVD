/* */
static void YUVFromRGB( uint32_t i_argb,
                    uint8_t *pi_y, uint8_t *pi_u, uint8_t *pi_v )
{
    int i_red   = ( i_argb & 0x00ff0000 ) >> 16;
    int i_green = ( i_argb & 0x0000ff00 ) >>  8;
    int i_blue  = ( i_argb & 0x000000ff );
    *pi_y = (uint8_t)__MIN(abs( 2104 * i_red  + 4130 * i_green +
                      802 * i_blue + 4096 + 131072 ) >> 13, 235);
    *pi_u = (uint8_t)__MIN(abs( -1214 * i_red  + -2384 * i_green +
                     3598 * i_blue + 4096 + 1048576) >> 13, 240);
    *pi_v = (uint8_t)__MIN(abs( 3598 * i_red + -3013 * i_green +
                      -585 * i_blue + 4096 + 1048576) >> 13, 240);
}
