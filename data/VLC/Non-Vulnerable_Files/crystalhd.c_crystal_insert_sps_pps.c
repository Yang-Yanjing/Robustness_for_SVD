/* Parse the SPS/PPS Metadata to feed the decoder for avc1 */
static int crystal_insert_sps_pps( decoder_t *p_dec,
                                   uint8_t *p_buf,
                                   uint32_t i_buf_size)
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int ret;
    p_sys->i_sps_pps_size = 0;
    p_sys->p_sps_pps_buf = malloc( p_dec->fmt_in.i_extra * 2 );
    if( !p_sys->p_sps_pps_buf )
        return VLC_ENOMEM;
    ret = convert_sps_pps( p_dec, p_buf, i_buf_size, p_sys->p_sps_pps_buf,
                           p_dec->fmt_in.i_extra * 2, &p_sys->i_sps_pps_size,
                           &p_sys->i_nal_size );
    if( !ret )
        return ret;
    free( p_sys->p_sps_pps_buf );
    p_sys->p_sps_pps_buf = NULL;
    return ret;
}
