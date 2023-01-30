 *****************************************************************************/
static int ProcessHeaders( decoder_t *p_dec )
{
    ogg_packet oggpacket;
    unsigned pi_size[XIPH_MAX_HEADER_COUNT];
    void     *pp_data[XIPH_MAX_HEADER_COUNT];
    unsigned i_count;
    if( xiph_SplitHeaders( pi_size, pp_data, &i_count,
                           p_dec->fmt_in.i_extra, p_dec->fmt_in.p_extra) )
        return VLC_EGENERIC;
    if( i_count < 2 )
        return VLC_EGENERIC;;
    oggpacket.granulepos = -1;
    oggpacket.e_o_s = 0;
    oggpacket.packetno = 0;
    /* Take care of the initial Opus header */
    oggpacket.b_o_s = 1; /* yes this actually is a b_o_s packet :) */
    oggpacket.bytes  = pi_size[0];
    oggpacket.packet = pp_data[0];
    int ret = ProcessInitialHeader( p_dec, &oggpacket );
    if (ret != VLC_SUCCESS)
        msg_Err( p_dec, "initial Opus header is corrupted" );
    return ret;
}
