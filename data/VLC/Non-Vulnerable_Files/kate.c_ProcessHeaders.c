 *****************************************************************************/
static int ProcessHeaders( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    kate_packet kp;
    unsigned pi_size[XIPH_MAX_HEADER_COUNT];
    void     *pp_data[XIPH_MAX_HEADER_COUNT];
    unsigned i_count;
    if( xiph_SplitHeaders( pi_size, pp_data, &i_count,
                           p_dec->fmt_in.i_extra, p_dec->fmt_in.p_extra) )
        return VLC_EGENERIC;
    if( i_count < 1 )
        return VLC_EGENERIC;
    /* Take care of the initial Kate header */
    kp.nbytes = pi_size[0];
    kp.data   = pp_data[0];
    int i_ret = kate_decode_headerin( &p_sys->ki, &p_sys->kc, &kp );
    if( i_ret < 0 )
    {
        msg_Err( p_dec, "this bitstream does not contain Kate data (%d)", i_ret );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_dec, "%s %s text, granule rate %f, granule shift %d",
             p_sys->ki.language, p_sys->ki.category,
             (double)p_sys->ki.gps_numerator/p_sys->ki.gps_denominator,
             p_sys->ki.granule_shift);
    /* parse all remaining header packets */
    for( unsigned i_headeridx = 1; i_headeridx < i_count; i_headeridx++ )
    {
        kp.nbytes = pi_size[i_headeridx];
        kp.data   = pp_data[i_headeridx];
        i_ret = kate_decode_headerin( &p_sys->ki, &p_sys->kc, &kp );
        if( i_ret < 0 )
        {
            msg_Err( p_dec, "Kate header %d is corrupted: %d", i_headeridx, i_ret );
            return VLC_EGENERIC;
        }
        /* header 1 is comments */
        if( i_headeridx == 1 )
        {
            ParseKateComments( p_dec );
        }
    }
#ifdef ENABLE_PACKETIZER
    if( !p_sys->b_packetizer )
#endif
    {
        /* We have all the headers, initialize decoder */
        msg_Dbg( p_dec, "we have all headers, initialize libkate for decoding" );
        i_ret = kate_decode_init( &p_sys->k, &p_sys->ki );
        if (i_ret < 0)
        {
            msg_Err( p_dec, "Kate failed to initialize for decoding: %d", i_ret );
            return VLC_EGENERIC;
        }
        p_sys->b_ready = true;
    }
#ifdef ENABLE_PACKETIZER
    else
    {
        p_dec->fmt_out.i_extra = p_dec->fmt_in.i_extra;
        p_dec->fmt_out.p_extra = xrealloc( p_dec->fmt_out.p_extra,
                                                  p_dec->fmt_out.i_extra );
        memcpy( p_dec->fmt_out.p_extra,
                p_dec->fmt_in.p_extra, p_dec->fmt_out.i_extra );
    }
#endif
    return VLC_SUCCESS;
}
