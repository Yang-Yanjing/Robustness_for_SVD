}
static void ParsePES( demux_t *p_demux )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    block_t     *p_pes = p_sys->p_pes;
    uint8_t     hdr[30];
    unsigned    i_skip;
    mtime_t     i_dts = -1;
    mtime_t     i_pts = -1;
    p_sys->p_pes = NULL;
    /* FIXME find real max size */
    block_ChainExtract( p_pes, hdr, 30 );
    /* See §2.4.3.6 of ISO 13818-1 */
    if( hdr[0] != 0 || hdr[1] != 0 || hdr[2] != 1 )
    {
        msg_Warn( p_demux, "invalid hdr [0x%2.2x:%2.2x:%2.2x:%2.2x]",
                  hdr[0], hdr[1],hdr[2],hdr[3] );
        block_ChainRelease( p_pes );
        return;
    }
    // hdr[4] i_pes_size, 2 bytes
    // hdr[6] Marker -> original_or_copy
    /* we assume mpeg2 PES */
    i_skip = hdr[8] + 9;
    if( hdr[7]&0x80 )    /* has pts */
    {
        i_pts = ((mtime_t)(hdr[ 9]&0x0e ) << 29)|
                 (mtime_t)(hdr[10] << 22)|
                ((mtime_t)(hdr[11]&0xfe) << 14)|
                 (mtime_t)(hdr[12] << 7)|
                 (mtime_t)(hdr[12] >> 1);
        if( hdr[7]&0x40 )    /* has dts */
        {
             i_dts = ((mtime_t)(hdr[14]&0x0e ) << 29)|
                     (mtime_t)(hdr[15] << 22)|
                    ((mtime_t)(hdr[16]&0xfe) << 14)|
                     (mtime_t)(hdr[17] << 7)|
                     (mtime_t)(hdr[18] >> 1);
        }
    }
    p_pes = block_ChainGather( p_pes );
    if( p_pes->i_buffer <= i_skip )
    {
        block_ChainRelease( p_pes );
        return;
    }
    p_pes->i_buffer -= i_skip;
    p_pes->p_buffer += i_skip;
    if( i_dts >= 0 )
        p_pes->i_dts = VLC_TS_0 + i_dts * 100 / 9;
    if( i_pts >= 0 )
        p_pes->i_pts = VLC_TS_0 + i_pts * 100 / 9;
    /* Set PCR */
    if( p_pes->i_pts > 0 )
    {
        es_out_Control( p_demux->out, ES_OUT_SET_PCR, (int64_t)p_pes->i_pts);
        p_sys->b_pcr_audio = true;
    }
    es_out_Send( p_demux->out, p_sys->p_audio, p_pes );
}
