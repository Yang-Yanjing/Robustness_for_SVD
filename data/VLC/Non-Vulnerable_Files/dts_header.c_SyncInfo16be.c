#include <assert.h>
static int SyncInfo16be( const uint8_t *p_buf,
                         unsigned int *pi_audio_mode,
                         unsigned int *pi_sample_rate,
                         unsigned int *pi_bit_rate,
                         unsigned int *pi_frame_length )
{
    unsigned int frame_size;
    unsigned int i_lfe;
    *pi_frame_length = (p_buf[4] & 0x01) << 6 | (p_buf[5] >> 2);
    frame_size = (p_buf[5] & 0x03) << 12 | (p_buf[6] << 4) | (p_buf[7] >> 4);
    *pi_audio_mode = (p_buf[7] & 0x0f) << 2 | (p_buf[8] >> 6);
    *pi_sample_rate = (p_buf[8] >> 2) & 0x0f;
    *pi_bit_rate = (p_buf[8] & 0x03) << 3 | ((p_buf[9] >> 5) & 0x07);
    i_lfe = (p_buf[10] >> 1) & 0x03;
    if( i_lfe ) *pi_audio_mode |= 0x10000;
    return frame_size + 1;
}
