}
static void HandleTelx( sout_stream_t *p_stream, block_t *p_block )
{
    sout_stream_sys_t *p_sys = (sout_stream_sys_t *)p_stream->p_sys;
    int len = p_block->i_buffer;
    for ( int offset = 1; offset + 46 <= len; offset += 46 )
    {
        uint8_t * packet = (uint8_t *) p_block->p_buffer+offset;
        if ( packet[0] == 0xFF )
            continue;
        int mpag = (hamming_8_4( packet[4] ) << 4) | hamming_8_4( packet[5] );
        int i_row, i_magazine;
        if ( mpag < 0 )
        {
            /* decode error */
            continue;
        }
        i_row = 0xFF & bytereverse(mpag);
        i_magazine = (7 & i_row) == 0 ? 8 : (7 & i_row);
        i_row >>= 3;
        if ( i_magazine != p_sys->i_magazine )
            continue;
        if ( i_row == 0 )
        {
            /* row 0 : flags and header line */
            p_sys->i_current_page =
                (0xF0 & bytereverse( hamming_8_4(packet[7]) )) |
                (0xF & (bytereverse( hamming_8_4(packet[6]) ) >> 4) );
        }
        if ( p_sys->i_current_page != p_sys->i_page ) continue;
        if ( i_row == p_sys->i_row )
        {
            /* row 1-23 : normal lines */
            char psz_line[41];
            decode_string( psz_line, packet + 6, 40 );
            psz_line[0] = tolower(psz_line[0]);
            psz_line[1] = tolower(psz_line[1]);
            psz_line[2] = tolower(psz_line[2]);
            psz_line[3] = '\0';
            SetLanguage( p_stream, psz_line );
        }
    }
}
