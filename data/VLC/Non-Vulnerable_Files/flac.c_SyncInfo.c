 *****************************************************************************/
static int SyncInfo(decoder_t *p_dec, uint8_t *p_buf,
                     unsigned int * pi_channels,
                     unsigned int * pi_sample_rate,
                     unsigned int * pi_bits_per_sample,
                     mtime_t * pi_pts,
                     mtime_t * pi_duration )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* Check syncword */
    if (p_buf[0] != 0xFF || (p_buf[1] & 0xFE) != 0xF8)
        return 0;
    /* Check there is no emulated sync code in the rest of the header */
    if (p_buf[2] == 0xff || p_buf[3] == 0xFF)
        return 0;
    /* Find blocksize (framelength) */
    int blocksize_hint = 0;
    unsigned blocksize = p_buf[2] >> 4;
    if (blocksize >= 8) {
        blocksize = 256 << (blocksize - 8);
    } else if (blocksize == 0) { /* value 0 is reserved */
        if (p_sys->b_stream_info &&
            p_sys->stream_info.min_blocksize == p_sys->stream_info.max_blocksize)
            blocksize = p_sys->stream_info.min_blocksize;
        else
            return 0; /* We can't do anything with this */
    } else if (blocksize == 1) {
        blocksize = 192;
    } else if (blocksize == 6 || blocksize == 7) {
        blocksize_hint = blocksize;
        blocksize = 0;
    } else /* 2, 3, 4, 5 */ {
        blocksize = 576 << (blocksize - 2);
    }
    if (p_sys->b_stream_info && !blocksize_hint )
        if (blocksize < p_sys->stream_info.min_blocksize ||
            blocksize > p_sys->stream_info.max_blocksize)
            return 0;
    /* Find samplerate */
    int samplerate_hint = p_buf[2] & 0xf;;
    unsigned int samplerate;
    if (samplerate_hint == 0) {
        if (p_sys->b_stream_info)
            samplerate = p_sys->stream_info.sample_rate;
        else
            return 0; /* We can't do anything with this */
    } else if (samplerate_hint == 15) {
        return 0; /* invalid */
    } else if (samplerate_hint < 12) {
        static const int16_t flac_samplerate[12] = {
            0,    8820, 17640, 19200,
            800,  1600, 2205,  2400,
            3200, 4410, 4800,  9600, 
        };
        samplerate = flac_samplerate[samplerate_hint] * 10;
    } else {
        samplerate = 0; /* at end of header */
    }
    /* Find channels */
    unsigned channels = p_buf[3] >> 4;
    if (channels >= 8) {
        if (channels >= 11) /* reserved */
            return 0;
        channels = 2;
    } else
        channels++;
    /* Find bits per sample */
    static const int8_t flac_bits_per_sample[8] = {
        0, 8, 12, -1, 16, 20, 24, -1
    };
    int bits_per_sample = flac_bits_per_sample[(p_buf[3] & 0x0e) >> 1];
    if (bits_per_sample == 0) {
        if (p_sys->b_stream_info)
            bits_per_sample = p_sys->stream_info.bits_per_sample;
        else
            return 0;
    } else if (bits_per_sample < 0)
        return 0;
    /* reserved for future use */
    if (p_buf[3] & 0x01)
        return 0;
    /* End of fixed size header */
    int i_header = 4;
    /* Check Sample/Frame number */
    int i_read;
    int64_t i_fsnumber = read_utf8(&p_buf[i_header++], &i_read);
    if ( i_fsnumber == INT64_MAX )
        return 0;
    i_header += i_read;
    /* Read blocksize */
    if (blocksize_hint) {
        blocksize = p_buf[i_header++];
        if (blocksize_hint == 7) {
            blocksize <<= 8;
            blocksize |= p_buf[i_header++];
        }
        blocksize++;
    }
    /* Read sample rate */
    if (samplerate == 0) {
        samplerate = p_buf[i_header++];
        if (samplerate_hint != 12) { /* 16 bits */
            samplerate <<= 8;
            samplerate |= p_buf[i_header++];
        }
        if (samplerate_hint == 12)
            samplerate *= 1000;
        else if (samplerate_hint == 14)
            samplerate *= 10;
    }
    if ( !samplerate )
        return 0;
    /* Check the CRC-8 byte */
    if (flac_crc8(p_buf, i_header) != p_buf[i_header])
        return 0;
    /* Sanity check using stream info header when possible */
    if (p_sys->b_stream_info) {
        if (blocksize < p_sys->stream_info.min_blocksize ||
            blocksize > p_sys->stream_info.max_blocksize)
            return 0;
        if ((unsigned)bits_per_sample != p_sys->stream_info.bits_per_sample)
            return 0;
        if (samplerate != p_sys->stream_info.sample_rate)
            return 0;
    }
    if( pi_pts )
    {
        *pi_pts = VLC_TS_0;
        if ( (p_buf[1] & 0x01) == 0  ) /* Fixed blocksize stream / Frames */
            *pi_pts += CLOCK_FREQ * blocksize * i_fsnumber / samplerate;
        else /* Variable blocksize stream / Samples */
            *pi_pts += CLOCK_FREQ * i_fsnumber / samplerate;
    }
    if ( pi_duration )
        *pi_duration = CLOCK_FREQ * blocksize / samplerate;
    *pi_bits_per_sample = bits_per_sample;
    *pi_sample_rate = samplerate;
    *pi_channels = channels;
    return blocksize;
}
