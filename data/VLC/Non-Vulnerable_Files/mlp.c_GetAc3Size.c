 */
static int GetAc3Size( const uint8_t *p_buf )
{
    static const int pi_rate[] = { 32,  40,  48,  56,  64,  80,  96, 112,
                                128, 160, 192, 224, 256, 320, 384, 448,
                                512, 576, 640 };
    /* */
    const int i_frmsizecod = p_buf[4] & 63;
    if( i_frmsizecod >= 38 )
        return 0;
    const int bitrate = pi_rate[i_frmsizecod >> 1];
    switch( p_buf[4] & 0xc0 )
    {
    case 0:
        return 4 * bitrate;
    case 0x40:
        return 2 * (320 * bitrate / 147 + (i_frmsizecod & 1));
    case 0x80:
        return 6 * bitrate;
    default:
        return 0;
    }
}
