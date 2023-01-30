 *****************************************************************************/
static picture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t   *p_sys = p_dec->p_sys;
    mpeg2_state_t   state;
    picture_t       *p_pic;
    block_t *p_block;
    if( !pp_block || !*pp_block )
        return NULL;
    p_block = *pp_block;
    if( p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY | BLOCK_FLAG_CORRUPTED) )
        Reset( p_dec );
    while( 1 )
    {
        state = mpeg2_parse( p_sys->p_mpeg2dec );
        switch( state )
        {
        case STATE_SEQUENCE:
        {
            /* */
            DpbClean( p_dec );
            /* */
            mpeg2_custom_fbuf( p_sys->p_mpeg2dec, 1 );
            /* Set the first 2 reference frames */
            p_sys->i_sar_num = 0;
            p_sys->i_sar_den = 0;
            GetAR( p_dec );
            for( int i = 0; i < 2; i++ )
            {
                picture_t *p_picture = DpbNewPicture( p_dec );
                if( !p_picture )
                {
                    Reset( p_dec );
                    block_Release( p_block );
                    return NULL;
                }
                PutPicture( p_dec, p_picture );
            }
            if( p_sys->p_synchro )
                decoder_SynchroRelease( p_sys->p_synchro );
            if( p_sys->p_info->sequence->frame_period <= 0 )
                p_sys->p_synchro = NULL;
            else
                p_sys->p_synchro =
                decoder_SynchroInit( p_dec, (uint32_t)(UINT64_C(1001000000) *
                                27 / p_sys->p_info->sequence->frame_period) );
            p_sys->b_after_sequence_header = true;
            break;
        }
        case STATE_GOP:
            /* There can be userdata in a GOP. It needs to be remembered for the next picture. */
            if( p_sys->p_info->user_data_len > 2 )
            {
                free( p_sys->p_gop_user_data );
                p_sys->p_gop_user_data = calloc( p_sys->p_info->user_data_len, sizeof(uint8_t) );
                if( p_sys->p_gop_user_data )
                {
                    p_sys->i_gop_user_data = p_sys->p_info->user_data_len;
                    memcpy( p_sys->p_gop_user_data, p_sys->p_info->user_data, p_sys->p_info->user_data_len );
                }
            }
            break;
        case STATE_PICTURE:
        {
            const mpeg2_info_t *p_info = p_sys->p_info;
            const mpeg2_picture_t *p_current = p_info->current_picture;
            mtime_t i_pts, i_dts;
            if( p_sys->b_after_sequence_header &&
                (p_current->flags &
                    PIC_MASK_CODING_TYPE) == PIC_FLAG_CODING_TYPE_P )
            {
                /* Intra-slice refresh. Simulate a blank I picture. */
                msg_Dbg( p_dec, "intra-slice refresh stream" );
                decoder_SynchroNewPicture( p_sys->p_synchro,
                                           I_CODING_TYPE, 2, 0, 0,
                                           p_info->sequence->flags & SEQ_FLAG_LOW_DELAY );
                decoder_SynchroDecode( p_sys->p_synchro );
                decoder_SynchroEnd( p_sys->p_synchro, I_CODING_TYPE, 0 );
                p_sys->b_slice_i = true;
            }
            p_sys->b_after_sequence_header = false;
#ifdef PIC_FLAG_PTS
            i_pts = p_current->flags & PIC_FLAG_PTS ?
                ( ( p_current->pts ==
                    (uint32_t)p_sys->i_current_pts ) ?
                  p_sys->i_current_pts : p_sys->i_previous_pts ) : 0;
            i_dts = 0;
            /* Hack to handle demuxers which only have DTS timestamps */
            if( !i_pts && !p_block->i_pts && p_block->i_dts > 0 )
            {
                if( p_info->sequence->flags & SEQ_FLAG_LOW_DELAY ||
                    (p_current->flags &
                      PIC_MASK_CODING_TYPE) == PIC_FLAG_CODING_TYPE_B )
                {
                    i_pts = p_block->i_dts;
                }
            }
            p_block->i_pts = p_block->i_dts = 0;
            /* End hack */
#else /* New interface */
            i_pts = p_current->flags & PIC_FLAG_TAGS ?
                ( ( p_current->tag == (uint32_t)p_sys->i_current_pts ) ?
                            p_sys->i_current_pts : p_sys->i_previous_pts ) : 0;
            i_dts = p_current->flags & PIC_FLAG_TAGS ?
                ( ( p_current->tag2 == (uint32_t)p_sys->i_current_dts ) ?
                            p_sys->i_current_dts : p_sys->i_previous_dts ) : 0;
#endif
            /* If nb_fields == 1, it is a field picture, and it will be
             * followed by another field picture for which we won't call
             * decoder_SynchroNewPicture() because this would have other
             * problems, so we take it into account here.
             * This kind of sucks, but I didn't think better. --Meuuh
             */
            decoder_SynchroNewPicture( p_sys->p_synchro,
                                       p_current->flags & PIC_MASK_CODING_TYPE,
                                       p_current->nb_fields == 1 ? 2 :
                                       p_current->nb_fields, i_pts, i_dts,
                                       p_info->sequence->flags & SEQ_FLAG_LOW_DELAY );
            bool b_skip = false;
            if( !p_dec->b_pace_control && !p_sys->b_preroll &&
                !(p_sys->b_slice_i
                   && ((p_current->flags
                         & PIC_MASK_CODING_TYPE) == PIC_FLAG_CODING_TYPE_P))
                   && !decoder_SynchroChoose( p_sys->p_synchro,
                              p_current->flags
                                & PIC_MASK_CODING_TYPE,
                              /*p_sys->p_vout->render_time*/ 0 /*FIXME*/,
                              p_info->sequence->flags & SEQ_FLAG_LOW_DELAY ) )
            {
                b_skip = true;
            }
            p_pic = NULL;
            if( !b_skip )
            {
                p_pic = DpbNewPicture( p_dec );
                if( !p_pic )
                {
                    Reset( p_dec );
                    p_pic = DpbNewPicture( p_dec );
                    if( !p_pic )
                    {
                        mpeg2_reset( p_sys->p_mpeg2dec, 1 );
                        block_Release( p_block );
                        return NULL;
                    }
                }
            }
            if( b_skip || !p_pic )
            {
                mpeg2_skip( p_sys->p_mpeg2dec, 1 );
                p_sys->b_skip = true;
                decoder_SynchroTrash( p_sys->p_synchro );
                PutPicture( p_dec, NULL );
                if( !b_skip )
                {
                    block_Release( p_block );
                    return NULL;
                }
            }
            else
            {
                mpeg2_skip( p_sys->p_mpeg2dec, 0 );
                p_sys->b_skip = false;
                decoder_SynchroDecode( p_sys->p_synchro );
                PutPicture( p_dec, p_pic );
            }
            if( p_info->user_data_len > 2 || p_sys->i_gop_user_data > 2 )
            {
                p_sys->i_cc_pts = i_pts;
                p_sys->i_cc_dts = i_dts;
                if( (p_current->flags
                             & PIC_MASK_CODING_TYPE) == PIC_FLAG_CODING_TYPE_P )
                    p_sys->i_cc_flags = BLOCK_FLAG_TYPE_P;
                else if( (p_current->flags
                             & PIC_MASK_CODING_TYPE) == PIC_FLAG_CODING_TYPE_B )
                    p_sys->i_cc_flags = BLOCK_FLAG_TYPE_B;
                else p_sys->i_cc_flags = BLOCK_FLAG_TYPE_I;
                bool b_top_field_first = p_sys->p_info->current_picture->flags
                                           & PIC_FLAG_TOP_FIELD_FIRST;
                if( p_sys->i_gop_user_data > 2 )
                {
                    /* We now have picture info for any cached user_data out of the gop */
                    cc_Extract( &p_sys->cc, b_top_field_first,
                                &p_sys->p_gop_user_data[0], p_sys->i_gop_user_data );
                    p_sys->i_gop_user_data = 0;
                }
                /* Extract the CC from the user_data of the picture */
                if( p_info->user_data_len > 2 )
                    cc_Extract( &p_sys->cc, b_top_field_first,
                                &p_info->user_data[0], p_info->user_data_len );
            }
        }
        break;
        case STATE_BUFFER:
            if( !p_block->i_buffer )
            {
                block_Release( p_block );
                return NULL;
            }
            if( (p_block->i_flags & (BLOCK_FLAG_DISCONTINUITY
                                      | BLOCK_FLAG_CORRUPTED)) &&
                p_sys->p_synchro &&
                p_sys->p_info->sequence &&
                p_sys->p_info->sequence->width != (unsigned)-1 )
            {
                decoder_SynchroReset( p_sys->p_synchro );
                if( p_sys->p_info->current_fbuf != NULL &&
                    p_sys->p_info->current_fbuf->id != NULL )
                {
                    p_sys->b_garbage_pic = true;
                }
                if( p_sys->b_slice_i )
                {
                    decoder_SynchroNewPicture( p_sys->p_synchro,
                                               I_CODING_TYPE, 2, 0, 0,
                                               p_sys->p_info->sequence->flags &
                                                            SEQ_FLAG_LOW_DELAY );
                    decoder_SynchroDecode( p_sys->p_synchro );
                    decoder_SynchroEnd( p_sys->p_synchro, I_CODING_TYPE, 0 );
                }
            }
            if( p_block->i_flags & BLOCK_FLAG_PREROLL )
            {
                p_sys->b_preroll = true;
            }
            else if( p_sys->b_preroll )
            {
                p_sys->b_preroll = false;
                if( p_sys->p_synchro )
                    decoder_SynchroReset( p_sys->p_synchro );
            }
#ifdef PIC_FLAG_PTS
            if( p_block->i_pts )
            {
                mpeg2_pts( p_sys->p_mpeg2dec, (uint32_t)p_block->i_pts );
#else /* New interface */
            if( p_block->i_pts || p_block->i_dts )
            {
                mpeg2_tag_picture( p_sys->p_mpeg2dec,
                                   (uint32_t)p_block->i_pts,
                                   (uint32_t)p_block->i_dts );
#endif
                p_sys->i_previous_pts = p_sys->i_current_pts;
                p_sys->i_current_pts = p_block->i_pts;
                p_sys->i_previous_dts = p_sys->i_current_dts;
                p_sys->i_current_dts = p_block->i_dts;
            }
            mpeg2_buffer( p_sys->p_mpeg2dec, p_block->p_buffer,
                          p_block->p_buffer + p_block->i_buffer );
            p_block->i_buffer = 0;
            break;
#if MPEG2_RELEASE >= MPEG2_VERSION (0, 5, 0)
        case STATE_SEQUENCE_MODIFIED:
            GetAR( p_dec );
            break;
#endif
        case STATE_PICTURE_2ND:
            p_sys->b_second_field = true;
            break;
        case STATE_INVALID_END:
        case STATE_END:
        case STATE_SLICE:
            p_pic = NULL;
            if( p_sys->p_info->display_fbuf &&
                p_sys->p_info->display_fbuf->id )
            {
                p_pic = p_sys->p_info->display_fbuf->id;
                if( DpbDisplayPicture( p_dec, p_pic ) )
                    p_pic = NULL;
                decoder_SynchroEnd( p_sys->p_synchro,
                                    p_sys->p_info->display_picture->flags & PIC_MASK_CODING_TYPE,
                                    p_sys->b_garbage_pic );
                if( p_pic )
                {
                    p_pic->date = decoder_SynchroDate( p_sys->p_synchro );
                    if( p_sys->b_garbage_pic )
                        p_pic->date = 0; /* ??? */
                    p_sys->b_garbage_pic = false;
                }
            }
            if( p_sys->p_info->discard_fbuf &&
                p_sys->p_info->discard_fbuf->id )
            {
                DpbUnlinkPicture( p_dec, p_sys->p_info->discard_fbuf->id );
            }
            /* For still frames */
            if( state == STATE_END && p_pic )
                p_pic->b_force = true;
            if( p_pic )
            {
                /* Avoid frames with identical timestamps.
                 * Especially needed for still frames in DVD menus. */
                if( p_sys->i_last_frame_pts == p_pic->date )
                    p_pic->date++;
                p_sys->i_last_frame_pts = p_pic->date;
                return p_pic;
            }
            break;
        case STATE_INVALID:
        {
            msg_Err( p_dec, "invalid picture encountered" );
            /* I don't think we have anything to do, but well without
             * docs ... */
            break;
        }
        default:
            break;
        }
    }
    /* Never reached */
    return NULL;
}

