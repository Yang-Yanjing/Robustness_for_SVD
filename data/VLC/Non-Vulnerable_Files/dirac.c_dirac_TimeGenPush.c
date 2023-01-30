 */
static int dirac_TimeGenPush( decoder_t *p_dec, block_t *p_block_in )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    dirac_block_encap_t *p_dbe;
    if( p_block_in->i_flags & BLOCK_FLAG_END_OF_SEQUENCE )
    {
        /* NB, this test occurs after the timegen push, so as to
         * push the block into the output queue */
        return 1;
    }
    if( p_block_in->i_flags & DIRAC_NON_DATED )
    {
        /* no picture found, which means p_block_in is a non-dated EU,
         * do not try and put a date on it */
        return 0;
    }
    p_dbe = dirac_GetBlockEncap( p_block_in );
    uint32_t u_picnum = p_dbe ? p_dbe->u_picture_number : 0;
    /*
     * Simple DTS regeneration:
     *  - DTS values linearly increase in stream order.
     *  - Every time a DTS occurs at the input, sync to it
     *    - If this is the first DTS seen, backdate all the previous ones that are undated
     *  - If a DTS is missing, guess that it increases by one picture period
     *  - If never seen DTS, don't do anything
     */
    /*
     * Simple PTS regeneration
     *  - PTS values do not linearly increase in stream order.
     *  - Every time a PTS occurs at the input, sync to it and record picture number
     *  - If a PTS is missing, guess that it differs by the product of picture
     *    period and difference between picture number of sync point and current picture
     *  - If this is the first PTS seen, backdate all previous ones that are undated
     *  - If never seen PTS, don't do anything
     */
    /*
     * Stage 1, sync to input timestamps, backdate timestamps for old
     * EUs that are in the outqueue with missing dates
     */
    if( p_block_in->i_dts > VLC_TS_INVALID )
    do {
        /* if timestamps exist, sync to them */
        if( p_sys->b_dts )
            break;
        /* first dts seen, backdate any packets in outqueue */
        p_sys->b_dts = true;
        date_t dts = p_sys->dts;
        dirac_BackdateDTS( p_sys->p_outqueue, p_block_in, &dts );
    } while( 0 );
    if( p_block_in->i_pts > VLC_TS_INVALID )
    do {
        /* if timestamps exist, sync to them */
        p_sys->u_pts_picnum = u_picnum;
        p_sys->i_pts = p_block_in->i_pts;
        if( p_sys->b_pts )
            break;
        /* first pts seen, backdate any packets in outqueue */
        p_sys->b_pts = true;
        date_t pts = p_sys->dts;
        date_Set( &pts, p_sys->i_pts );
        dirac_BackdatePTS( p_sys->p_outqueue, p_block_in, &pts, p_sys->u_pts_picnum );
    } while( 0 );
    /*
     * Stage 2, don't attempt to forwards interpolate timestamps for
     * blocks if the picture rates aren't known
     */
    if( !p_sys->b_seen_seq_hdr )
    {
        return 0;
    }
    /*
     * Stage 3, for block_in, interpolate any missing timestamps
     */
    if( p_sys->b_dts && p_block_in->i_dts <= VLC_TS_INVALID )
    {
        /* dts has previously been seen, but not this time, interpolate */
        p_block_in->i_dts = date_Increment( &p_sys->dts, 1 );
    }
    if( p_sys->b_pts && p_block_in->i_pts <= VLC_TS_INVALID )
    {
        /* pts has previously been seen, but not this time, interpolate */
        date_t pts = p_sys->dts;
        date_Set( &pts, p_sys->i_pts );
        int32_t i_dist = u_picnum - p_sys->u_pts_picnum;
        if( i_dist >= 0 )
            p_block_in->i_pts = date_Increment( &pts, i_dist );
        else
            p_block_in->i_pts = date_Decrement( &pts, -i_dist );
    }
    /* If pts and dts have been seen, there is no need to simulate operation
     * of the decoder reorder buffer */
    /* If neither have been seen, there is little point in simulating */
    if( p_sys->b_dts == p_sys->b_pts )
        return 0;
    /* model the reorder buffer */
    block_t *p_block = dirac_Reorder( p_dec, p_block_in, u_picnum );
    if( !p_block )
        return 0;
    /* A future ehancement is to stop modeling the reorder buffer as soon as
     * the first packet is output -- interpolate the past and freewheel for
     * the future */
    p_dbe = dirac_GetBlockEncap( p_block );
    u_picnum = p_dbe ? p_dbe->u_picture_number : 0;
    if( p_sys->b_tg_last_picnum )
    {
        if( dirac_PictureNbeforeM( u_picnum, p_sys->u_tg_last_picnum ) )
        {
            msg_Warn( p_dec, "stream jumped? %d < %d: resetting"
                    , u_picnum, p_sys->u_tg_last_picnum );
            /* pictures only emerge from the reorder buffer in sequence
             * if a stream suddenly jumped backwards without a signaling
             * a discontinuity, some pictures will get stuck in the RoB.
             * flush the RoB. */
            /* this could be a bit less indiscriminate */
            p_dbe = dirac_GetBlockEncap( p_sys->p_outqueue );
            uint32_t u_prev_parse_offset = p_dbe ? p_dbe->u_last_next_offset : 0;
            block_ChainRelease( p_sys->p_outqueue );
            p_sys->p_outqueue = dirac_EmitEOS( p_dec, u_prev_parse_offset );
            if( p_sys->p_outqueue )
                p_sys->p_outqueue->i_flags = BLOCK_FLAG_DISCONTINUITY | DIRAC_NON_DATED;
            /* return 2, so as not to reset the b_dts flags -- needed if
             * using the rawdirac demuxer with broken stream */
            return 2;
        }
    }
    p_sys->b_tg_last_picnum = true;
    p_sys->u_tg_last_picnum = u_picnum;
    return 0;
}
