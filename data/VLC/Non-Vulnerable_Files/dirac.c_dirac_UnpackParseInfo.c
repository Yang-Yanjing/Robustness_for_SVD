 */
static bool dirac_UnpackParseInfo( parse_info_t *p_pi, block_bytestream_t *p_bs,
                                   size_t u_offset )
{
    uint8_t p_d[13];
    if( VLC_SUCCESS != block_PeekOffsetBytes( p_bs, u_offset, p_d, 13 ) )
        return false;
    if( p_d[0] != 'B' || p_d[1] != 'B' || p_d[2] != 'C' || p_d[3] != 'D' )
        return false;
    p_pi->i_parse_code = p_d[4];
    p_pi->u_next_offset = p_d[5] << 24 | p_d[6] << 16 | p_d[7] << 8 | p_d[8];
    p_pi->u_prev_offset = p_d[9] << 24 | p_d[10] << 16 | p_d[11] << 8 | p_d[12];
    return true;
}
