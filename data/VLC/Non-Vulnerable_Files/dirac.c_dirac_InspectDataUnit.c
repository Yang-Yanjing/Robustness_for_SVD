 */
static int dirac_InspectDataUnit( decoder_t *p_dec, block_t **pp_block, block_t *p_eu )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block = *pp_block;
    uint8_t u_parse_code = p_block->p_buffer[4];
    if( dirac_isEOS( u_parse_code ) )
    {
        if( p_sys->b_seen_eos )
        {
            /* remove duplicate EOS packets */
            block_Release( p_block );
            *pp_block = NULL;
            return DIRAC_DU_IN_EU;
        }
        /* p_block is an EOS packet */
        p_eu->i_flags |= BLOCK_FLAG_END_OF_SEQUENCE;
        /* for the moment, let this end an encapsulation unit */
        /* seeing an eos packet requires a flush of the packetizer
         * this is detected by the caller of this function */
        p_sys->b_seen_seq_hdr = false;
        p_sys->b_seen_eos = true;
        return DIRAC_DU_ENDS_EU;
#if 0
        /* let anything down streem know too */
        /*
        Actually, this is a bad idea:
         - It sets the discontinuity for every dirac EOS packet
           which doesnt imply a time discontinuity.
         - When the synchronizer detects a real discontinuity, it
           should copy the flags through.
        p_eu->i_flags |= BLOCK_FLAG_DISCONTINUITY;
        */
#endif
    }
    p_sys->b_seen_eos = false;
    if( dirac_isPicture( u_parse_code ) )
    {
        /* timestamps apply to pictures only */
        p_eu->i_dts = p_sys->i_eu_dts;
        p_eu->i_pts = p_sys->i_eu_pts;
        p_sys->i_eu_dts = p_sys->i_eu_pts = VLC_TS_INVALID;
        if( !p_sys->b_seen_seq_hdr )
        {
            /* can't timestamp in this case, discard later
             * so that the timestamps aren't lost */
            p_eu->i_flags |= DIRAC_DISCARD;
        }
        /* p_block is a picture -- it ends the 'encapsulation unit' */
        if( dirac_numRefs( u_parse_code ) )
        {
            /* if this picture is not an I frame, ensure that the
             * random access point flags are not set */
            p_eu->i_flags &= ~BLOCK_FLAG_TYPE_I;
        }
        dirac_block_encap_t *p_dbe = dirac_GetBlockEncap( p_block );
        if( p_dbe && p_block->i_buffer > 13+4 )
        {
            /* record the picture number to save the time gen functions
             * from having to inspect the data for it */
            p_dbe->u_picture_number = GetDWBE( p_block->p_buffer + 13 );
        }
        return DIRAC_DU_ENDS_EU;
    }
    if( dirac_isSeqHdr( u_parse_code ) )
    {
        if( !dirac_UnpackSeqHdr( &p_sys->seq_hdr, p_block ) )
        {
            /* couldn't parse the sequence header, just ignore it */
            return DIRAC_DU_IN_EU;
        }
        p_sys->b_seen_seq_hdr = true;
       /* a sequence header followed by an I frame is a random
        * access point; assume that this is the case */
        p_eu->i_flags |= BLOCK_FLAG_TYPE_I;
        es_format_t *p_es = &p_dec->fmt_out;
        p_es->video.i_width  = p_sys->seq_hdr.u_width;
        p_es->video.i_height = p_sys->seq_hdr.u_height;
        vlc_ureduce( &p_es->video.i_frame_rate, &p_es->video.i_frame_rate_base
                   , p_sys->seq_hdr.u_fps_num, p_sys->seq_hdr.u_fps_den, 0 );
        /* when field coding, dts needs to be incremented in terms of field periods */
        unsigned u_pics_per_sec = p_sys->seq_hdr.u_fps_num;
        if( p_sys->seq_hdr.u_picture_coding_mode == DIRAC_FIELD_CODING )
        {
            u_pics_per_sec *= 2;
        }
        date_Change( &p_sys->dts, u_pics_per_sec, p_sys->seq_hdr.u_fps_den );
        /* TODO: set p_sys->reorder_buf.u_size_max */
        p_sys->i_pts_offset = p_sys->reorder_buf.u_size_max
                            * 1000000
                            * p_es->video.i_frame_rate_base / p_es->video.i_frame_rate + 1;
        /* stash a copy of the seqhdr
         *  - required for ogg muxing
         *  - useful for error checking
         *  - it isn't allowed to change until an eos */
        free( p_es->p_extra );
        p_es->p_extra = calloc( 1, p_block->i_buffer + 13 );
        if( !p_es->p_extra )
        {
            p_es->i_extra = 0;
            return DIRAC_DU_IN_EU;
        }
        p_es->i_extra = p_block->i_buffer;
        memcpy( p_es->p_extra, p_block->p_buffer, p_block->i_buffer );
        /* append EOS as per Ogg guidelines */
        p_block = dirac_EmitEOS( p_dec, p_block->i_buffer );
        if( p_block )
        {
            memcpy( (uint8_t*)p_es->p_extra + p_es->i_extra, p_block->p_buffer, 13 );
            p_es->i_extra += 13;
        }
        return DIRAC_DU_IN_EU;
    }
    /* doesn't end an encapsulation unit */
    return DIRAC_DU_IN_EU;
}
