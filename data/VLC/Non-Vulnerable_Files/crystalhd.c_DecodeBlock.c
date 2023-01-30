 ****************************************************************************/
static picture_t *DecodeBlock( decoder_t *p_dec, block_t **pp_block )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    block_t *p_block;
    BC_DTS_PROC_OUT proc_out;
    BC_DTS_STATUS driver_stat;
    /* First check the status of the decode to produce pictures */
    if( BC_FUNC_PSYS(DtsGetDriverStatus)( p_sys->bcm_handle, &driver_stat ) != BC_STS_SUCCESS )
        return NULL;
    p_block = *pp_block;
    if( p_block )
    {
        if( ( p_block->i_flags&(BLOCK_FLAG_DISCONTINUITY|BLOCK_FLAG_CORRUPTED) ) == 0 )
        {
            /* Valid input block, so we can send to HW to decode */
            BC_STATUS status = BC_FUNC_PSYS(DtsProcInput)( p_sys->bcm_handle,
                                            p_block->p_buffer,
                                            p_block->i_buffer,
                                            p_block->i_pts >= VLC_TS_INVALID ? TO_BC_PTS(p_block->i_pts) : 0, false );
            block_Release( p_block );
            *pp_block = NULL;
            if( status != BC_STS_SUCCESS )
                return NULL;
        }
    }
#ifdef DEBUG_CRYSTALHD
    else
    {
        if( driver_stat.ReadyListCount != 0 )
            msg_Err( p_dec, " Input NULL but have pictures %u", driver_stat.ReadyListCount );
    }
#endif
    if( driver_stat.ReadyListCount == 0 )
        return NULL;
    /* Prepare the Output structure */
    /* We always expect and use YUY2 */
    memset( &proc_out, 0, sizeof(BC_DTS_PROC_OUT) );
    proc_out.PicInfo.width  = p_dec->fmt_out.video.i_width;
    proc_out.PicInfo.height = p_dec->fmt_out.video.i_height;
    proc_out.PoutFlags      = BC_POUT_FLAGS_SIZE;
    proc_out.AppCallBack    = ourCallback;
    proc_out.hnd            = p_dec;
    p_sys->proc_out         = &proc_out;
    /* */
    BC_STATUS sts = BC_FUNC_PSYS(DtsProcOutput)( p_sys->bcm_handle, 128, &proc_out );
#ifdef DEBUG_CRYSTALHD
    if( sts != BC_STS_SUCCESS )
        msg_Err( p_dec, "DtsProcOutput returned %i", sts );
#endif
    uint8_t b_eos;
    picture_t *p_pic = p_sys->p_pic;
    switch( sts )
    {
        case BC_STS_SUCCESS:
            if( !(proc_out.PoutFlags & BC_POUT_FLAGS_PIB_VALID) )
            {
                msg_Dbg( p_dec, "Invalid PIB" );
                break;
            }
            if( !p_pic )
                break;
            /* In interlaced mode, do not push the first field in the pipeline */
            if( (proc_out.PicInfo.flags & VDEC_FLAG_INTERLACED_SRC) &&
               !(proc_out.PicInfo.flags & VDEC_FLAG_FIELDPAIR) )
                return NULL;
            //  crystal_CopyPicture( p_pic, &proc_out );
            p_pic->date = proc_out.PicInfo.timeStamp > 0 ?
                          FROM_BC_PTS(proc_out.PicInfo.timeStamp) : VLC_TS_INVALID;
            //p_pic->date += 100 * 1000;
#ifdef DEBUG_CRYSTALHD
            msg_Dbg( p_dec, "TS Output is %"PRIu64, p_pic->date);
#endif
            return p_pic;
        case BC_STS_DEC_NOT_OPEN:
        case BC_STS_DEC_NOT_STARTED:
            msg_Err( p_dec, "Decoder not opened or started" );
            break;
        case BC_STS_INV_ARG:
            msg_Warn( p_dec, "Invalid arguments. Please report" );
            break;
        case BC_STS_FMT_CHANGE:    /* Format change */
            /* if( !(proc_out.PoutFlags & BC_POUT_FLAGS_PIB_VALID) )
                break; */
            p_dec->fmt_out.video.i_width  = proc_out.PicInfo.width;
            p_dec->fmt_out.video.i_height = proc_out.PicInfo.height;
            if( proc_out.PicInfo.height == 1088 )
                p_dec->fmt_out.video.i_height = 1080;
#define setAR( a, b, c ) case a: p_dec->fmt_out.video.i_sar_num = b; \
                                 p_dec->fmt_out.video.i_sar_den = c; break;
            switch( proc_out.PicInfo.aspect_ratio )
            {
                setAR( vdecAspectRatioSquare, 1, 1 )
                setAR( vdecAspectRatio12_11, 12, 11 )
                setAR( vdecAspectRatio10_11, 10, 11 )
                setAR( vdecAspectRatio16_11, 16, 11 )
                setAR( vdecAspectRatio40_33, 40, 33 )
                setAR( vdecAspectRatio24_11, 24, 11 )
                setAR( vdecAspectRatio20_11, 20, 11 )
                setAR( vdecAspectRatio32_11, 32, 11 )
                setAR( vdecAspectRatio80_33, 80, 33 )
                setAR( vdecAspectRatio18_11, 18, 11 )
                setAR( vdecAspectRatio15_11, 15, 11 )
                setAR( vdecAspectRatio64_33, 64, 33 )
                setAR( vdecAspectRatio160_99, 160, 99 )
                setAR( vdecAspectRatio4_3, 4, 3 )
                setAR( vdecAspectRatio16_9, 16, 9 )
                setAR( vdecAspectRatio221_1, 221, 1 )
                default: break;
            }
#undef setAR
            msg_Dbg( p_dec, "Format Change Detected [%i, %i], AR: %i/%i",
                    proc_out.PicInfo.width, proc_out.PicInfo.height,
                    p_dec->fmt_out.video.i_sar_num,
                    p_dec->fmt_out.video.i_sar_den );
            break;
        /* Nothing is documented here... */
        case BC_STS_NO_DATA:
            if( BC_FUNC_PSYS(DtsIsEndOfStream)( p_sys->bcm_handle, &b_eos )
                    == BC_STS_SUCCESS )
                if( b_eos )
                    msg_Dbg( p_dec, "End of Stream" );
            break;
        case BC_STS_TIMEOUT:       /* Timeout */
            msg_Err( p_dec, "ProcOutput timeout" );
            break;
        case BC_STS_IO_XFR_ERROR:
        case BC_STS_IO_USER_ABORT:
        case BC_STS_IO_ERROR:
            msg_Err( p_dec, "ProcOutput return mode not implemented. Please report" );
            break;
        default:
            msg_Err( p_dec, "Unknown return status. Please report %i", sts );
            break;
    }
    if( p_pic )
        decoder_DeletePicture( p_dec, p_pic );
    return NULL;
}
