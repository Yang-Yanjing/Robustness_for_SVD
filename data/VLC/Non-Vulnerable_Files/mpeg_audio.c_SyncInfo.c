 *****************************************************************************/
static int SyncInfo( uint32_t i_header, unsigned int * pi_channels,
                     unsigned int * pi_channels_conf,
                     unsigned int * pi_sample_rate, unsigned int * pi_bit_rate,
                     unsigned int * pi_frame_length,
                     unsigned int * pi_max_frame_size, unsigned int * pi_layer)
{
    static const int ppi_bitrate[2][3][16] =
    {
        {
            /* v1 l1 */
            { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384,
              416, 448, 0},
            /* v1 l2 */
            { 0, 32, 48, 56,  64,  80,  96, 112, 128, 160, 192, 224, 256,
              320, 384, 0},
            /* v1 l3 */
            { 0, 32, 40, 48,  56,  64,  80,  96, 112, 128, 160, 192, 224,
              256, 320, 0}
        },
        {
            /* v2 l1 */
            { 0, 32, 48, 56,  64,  80,  96, 112, 128, 144, 160, 176, 192,
              224, 256, 0},
            /* v2 l2 */
            { 0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128,
              144, 160, 0},
            /* v2 l3 */
            { 0,  8, 16, 24,  32,  40,  48,  56,  64,  80,  96, 112, 128,
              144, 160, 0}
        }
    };
    static const int ppi_samplerate[2][4] = /* version 1 then 2 */
    {
        { 44100, 48000, 32000, 0 },
        { 22050, 24000, 16000, 0 }
    };
    int i_version, i_mode, i_emphasis;
    bool b_padding, b_mpeg_2_5;
    int i_frame_size = 0;
    int i_bitrate_index, i_samplerate_index;
    int i_max_bit_rate;
    b_mpeg_2_5  = 1 - ((i_header & 0x100000) >> 20);
    i_version   = 1 - ((i_header & 0x80000) >> 19);
    *pi_layer   = 4 - ((i_header & 0x60000) >> 17);
    //bool b_crc = !((i_header >> 16) & 0x01);
    i_bitrate_index = (i_header & 0xf000) >> 12;
    i_samplerate_index = (i_header & 0xc00) >> 10;
    b_padding   = (i_header & 0x200) >> 9;
    /* Extension */
    i_mode      = (i_header & 0xc0) >> 6;
    /* Modeext, copyright & original */
    i_emphasis  = i_header & 0x3;
    if( *pi_layer != 4 &&
        i_bitrate_index < 0x0f &&
        i_samplerate_index != 0x03 &&
        i_emphasis != 0x02 )
    {
        switch ( i_mode )
        {
        case 0: /* stereo */
        case 1: /* joint stereo */
            *pi_channels = 2;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT;
            break;
        case 2: /* dual-mono */
            *pi_channels = 2;
            *pi_channels_conf = AOUT_CHAN_LEFT | AOUT_CHAN_RIGHT
                                | AOUT_CHAN_DUALMONO;
            break;
        case 3: /* mono */
            *pi_channels = 1;
            *pi_channels_conf = AOUT_CHAN_CENTER;
            break;
        }
        *pi_bit_rate = ppi_bitrate[i_version][*pi_layer-1][i_bitrate_index];
        i_max_bit_rate = ppi_bitrate[i_version][*pi_layer-1][14];
        *pi_sample_rate = ppi_samplerate[i_version][i_samplerate_index];
        if ( b_mpeg_2_5 )
        {
            *pi_sample_rate >>= 1;
        }
        switch( *pi_layer )
        {
        case 1:
            i_frame_size = ( 12000 * *pi_bit_rate / *pi_sample_rate +
                           b_padding ) * 4;
            *pi_max_frame_size = ( 12000 * i_max_bit_rate /
                                 *pi_sample_rate + 1 ) * 4;
            *pi_frame_length = 384;
            break;
        case 2:
            i_frame_size = 144000 * *pi_bit_rate / *pi_sample_rate + b_padding;
            *pi_max_frame_size = 144000 * i_max_bit_rate / *pi_sample_rate + 1;
            *pi_frame_length = 1152;
            break;
        case 3:
            i_frame_size = ( i_version ? 72000 : 144000 ) *
                           *pi_bit_rate / *pi_sample_rate + b_padding;
            *pi_max_frame_size = ( i_version ? 72000 : 144000 ) *
                                 i_max_bit_rate / *pi_sample_rate + 1;
            *pi_frame_length = i_version ? 576 : 1152;
            break;
        default:
            break;
        }
        /* Free bitrate mode can support higher bitrates */
        if( !*pi_bit_rate ) *pi_max_frame_size *= 2;
    }
    else
    {
        return -1;
    }
    return i_frame_size;
}
