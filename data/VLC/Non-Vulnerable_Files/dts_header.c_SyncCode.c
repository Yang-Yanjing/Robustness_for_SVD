}
int SyncCode( const uint8_t *p_buf )
{
    /* 14 bits, little endian version of the bitstream */
    if( p_buf[0] == 0xff && p_buf[1] == 0x1f &&
        p_buf[2] == 0x00 && p_buf[3] == 0xe8 &&
        (p_buf[4] & 0xf0) == 0xf0 && p_buf[5] == 0x07 )
    {
        return VLC_SUCCESS;
    }
    /* 14 bits, big endian version of the bitstream */
    else if( p_buf[0] == 0x1f && p_buf[1] == 0xff &&
             p_buf[2] == 0xe8 && p_buf[3] == 0x00 &&
             p_buf[4] == 0x07 && (p_buf[5] & 0xf0) == 0xf0 )
    {
        return VLC_SUCCESS;
    }
    /* 16 bits, big endian version of the bitstream */
    else if( p_buf[0] == 0x7f && p_buf[1] == 0xfe &&
             p_buf[2] == 0x80 && p_buf[3] == 0x01 )
    {
        return VLC_SUCCESS;
    }
    /* 16 bits, little endian version of the bitstream */
    else if( p_buf[0] == 0xfe && p_buf[1] == 0x7f &&
             p_buf[2] == 0x01 && p_buf[3] == 0x80 )
    {
        return VLC_SUCCESS;
    }
    /* DTS-HD */
    else if( p_buf[0] == 0x64 && p_buf[1] ==  0x58 &&
             p_buf[2] == 0x20 && p_buf[3] ==  0x25 )
    {
        return VLC_SUCCESS;
    }
    return VLC_EGENERIC;
}
