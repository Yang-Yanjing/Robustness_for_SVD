 *****************************************************************************/
static int ProcessHeaders( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
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
    /* Take care of the initial Vorbis header */
    oggpacket.b_o_s = 1; /* yes this actually is a b_o_s packet :) */
    oggpacket.bytes  = pi_size[0];
    oggpacket.packet = pp_data[0];
    if( ProcessInitialHeader( p_dec, &oggpacket ) != VLC_SUCCESS )
    {
        msg_Err( p_dec, "initial Speex header is corrupted" );
        return VLC_EGENERIC;;
    }
    /* The next packet in order is the comments header */
    oggpacket.b_o_s = 0;
    oggpacket.bytes  = pi_size[1];
    oggpacket.packet = pp_data[1];
    ParseSpeexComments( p_dec, &oggpacket );
    if( p_sys->b_packetizer )
    {
        p_dec->fmt_out.i_extra = p_dec->fmt_in.i_extra;
        p_dec->fmt_out.p_extra = xrealloc( p_dec->fmt_out.p_extra,
                                                  p_dec->fmt_out.i_extra );
        memcpy( p_dec->fmt_out.p_extra,
                p_dec->fmt_in.p_extra, p_dec->fmt_out.i_extra );
    }
    return VLC_SUCCESS;
}
