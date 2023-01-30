 *****************************************************************************/
static int ParseControlSeq( decoder_t *p_dec, subpicture_t *p_spu,
                            subpicture_data_t *p_spu_data, spu_properties_t *p_spu_properties, mtime_t i_pts )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    /* Our current index in the SPU packet */
    unsigned int i_index;
    /* The next start-of-control-sequence index and the previous one */
    unsigned int i_next_seq = 0, i_cur_seq = 0;
    /* Command and date */
    uint8_t i_command = SPU_CMD_END;
    mtime_t date = 0;
    bool b_cmd_offset = false;
    bool b_cmd_alpha = false;
    subpicture_data_t spu_data_cmd;
    if( !p_spu || !p_spu_data )
        return VLC_EGENERIC;
    /* Create working space for spu data */
    memset( &spu_data_cmd, 0, sizeof(spu_data_cmd) );
    spu_data_cmd.pi_offset[0] = -1;
    spu_data_cmd.pi_offset[1] = -1;
    spu_data_cmd.p_data = NULL;
    spu_data_cmd.b_palette = false;
    spu_data_cmd.b_auto_crop = false;
    spu_data_cmd.i_y_top_offset = 0;
    spu_data_cmd.i_y_bottom_offset = 0;
    spu_data_cmd.pi_alpha[0] = 0x00;
    spu_data_cmd.pi_alpha[1] = 0x0f;
    spu_data_cmd.pi_alpha[2] = 0x0f;
    spu_data_cmd.pi_alpha[3] = 0x0f;
    /* Initialize the structure */
    p_spu->i_start = p_spu->i_stop = 0;
    p_spu->b_ephemer = false;
    memset( p_spu_properties, 0, sizeof(*p_spu_properties) );
    /* */
    *p_spu_data = spu_data_cmd;
    for( i_index = 4 + p_sys->i_rle_size; i_index < p_sys->i_spu_size ; )
    {
        /* If we just read a command sequence, read the next one;
         * otherwise, go on with the commands of the current sequence. */
        if( i_command == SPU_CMD_END )
        {
            if( i_index + 4 > p_sys->i_spu_size )
            {
                msg_Err( p_dec, "overflow in SPU command sequence" );
                return VLC_EGENERIC;
            }
            /* */
            b_cmd_offset = false;
            b_cmd_alpha = false;
            /* Get the control sequence date */
            date = (mtime_t)GetWBE( &p_sys->buffer[i_index] ) * 11000;
            /* Next offset */
            i_cur_seq = i_index;
            i_next_seq = GetWBE( &p_sys->buffer[i_index+2] );
            if( i_next_seq > p_sys->i_spu_size )
            {
                msg_Err( p_dec, "overflow in SPU next command sequence" );
                return VLC_EGENERIC;
            }
            /* Skip what we just read */
            i_index += 4;
        }
        i_command = p_sys->buffer[i_index];
        switch( i_command )
        {
        case SPU_CMD_FORCE_DISPLAY: /* 00 (force displaying) */
            p_spu->i_start = i_pts + date;
            p_spu->b_ephemer = true;
            i_index += 1;
            break;
        /* Convert the dates in seconds to PTS values */
        case SPU_CMD_START_DISPLAY: /* 01 (start displaying) */
            p_spu->i_start = i_pts + date;
            i_index += 1;
            break;
        case SPU_CMD_STOP_DISPLAY: /* 02 (stop displaying) */
            p_spu->i_stop = i_pts + date;
            i_index += 1;
            break;
        case SPU_CMD_SET_PALETTE:
            /* 03xxxx (palette) */
            if( i_index + 3 > p_sys->i_spu_size )
            {
                msg_Err( p_dec, "overflow in SPU command" );
                return VLC_EGENERIC;
            }
            if( p_dec->fmt_in.subs.spu.palette[0] == 0xBeeF )
            {
                unsigned int idx[4];
                int i;
                spu_data_cmd.b_palette = true;
                idx[0] = (p_sys->buffer[i_index+1]>>4)&0x0f;
                idx[1] = (p_sys->buffer[i_index+1])&0x0f;
                idx[2] = (p_sys->buffer[i_index+2]>>4)&0x0f;
                idx[3] = (p_sys->buffer[i_index+2])&0x0f;
                for( i = 0; i < 4 ; i++ )
                {
                    uint32_t i_color = p_dec->fmt_in.subs.spu.palette[1+idx[i]];
                    /* FIXME: this job should be done sooner */
                    spu_data_cmd.pi_yuv[3-i][0] = (i_color>>16) & 0xff;
                    spu_data_cmd.pi_yuv[3-i][1] = (i_color>>0) & 0xff;
                    spu_data_cmd.pi_yuv[3-i][2] = (i_color>>8) & 0xff;
                }
            }
            i_index += 3;
            break;
        case SPU_CMD_SET_ALPHACHANNEL: /* 04xxxx (alpha channel) */
            if( i_index + 3 > p_sys->i_spu_size )
            {
                msg_Err( p_dec, "overflow in SPU command" );
                return VLC_EGENERIC;
            }
            if(!p_sys->b_disabletrans)
            { /* If we want to use original transparency values */
                b_cmd_alpha = true;
                spu_data_cmd.pi_alpha[3] = (p_sys->buffer[i_index+1]>>4)&0x0f;
                spu_data_cmd.pi_alpha[2] = (p_sys->buffer[i_index+1])&0x0f;
                spu_data_cmd.pi_alpha[1] = (p_sys->buffer[i_index+2]>>4)&0x0f;
                spu_data_cmd.pi_alpha[0] = (p_sys->buffer[i_index+2])&0x0f;
            }
            i_index += 3;
            break;
        case SPU_CMD_SET_COORDINATES: /* 05xxxyyyxxxyyy (coordinates) */
            if( i_index + 7 > p_sys->i_spu_size )
            {
                msg_Err( p_dec, "overflow in SPU command" );
                return VLC_EGENERIC;
            }
            p_spu_properties->i_x = (p_sys->buffer[i_index+1]<<4)|
                         ((p_sys->buffer[i_index+2]>>4)&0x0f);
            p_spu_properties->i_width = (((p_sys->buffer[i_index+2]&0x0f)<<8)|
                              p_sys->buffer[i_index+3]) - p_spu_properties->i_x + 1;
            p_spu_properties->i_y = (p_sys->buffer[i_index+4]<<4)|
                         ((p_sys->buffer[i_index+5]>>4)&0x0f);
            p_spu_properties->i_height = (((p_sys->buffer[i_index+5]&0x0f)<<8)|
                              p_sys->buffer[i_index+6]) - p_spu_properties->i_y + 1;
            /* Auto crop fullscreen subtitles */
            if( p_spu_properties->i_height > 250 )
                p_spu_data->b_auto_crop = true;
            i_index += 7;
            break;
        case SPU_CMD_SET_OFFSETS: /* 06xxxxyyyy (byte offsets) */
            if( i_index + 5 > p_sys->i_spu_size )
            {
                msg_Err( p_dec, "overflow in SPU command" );
                return VLC_EGENERIC;
            }
            b_cmd_offset = true;
            p_spu_data->pi_offset[0] = GetWBE(&p_sys->buffer[i_index+1]) - 4;
            p_spu_data->pi_offset[1] = GetWBE(&p_sys->buffer[i_index+3]) - 4;
            i_index += 5;
            break;
        case SPU_CMD_END: /* ff (end) */
            if( b_cmd_offset )
            {
                /* It seems that palette and alpha from the block having
                 * the cmd offset have to be used
                 * XXX is it all ? */
                p_spu_data->b_palette = spu_data_cmd.b_palette;
                if( spu_data_cmd.b_palette )
                    memcpy( p_spu_data->pi_yuv, spu_data_cmd.pi_yuv, sizeof(spu_data_cmd.pi_yuv) );
                if( b_cmd_alpha )
                    memcpy( p_spu_data->pi_alpha, spu_data_cmd.pi_alpha, sizeof(spu_data_cmd.pi_alpha) );
            }
            i_index += 1;
            break;
        default: /* xx (unknown command) */
            msg_Warn( p_dec, "unknown SPU command 0x%.2x", i_command );
            if( i_index + 1 < i_next_seq )
            {
                 /* There is at least one other command sequence */
                 if( p_sys->buffer[i_next_seq - 1] == SPU_CMD_END )
                 {
                     /* This is consistent. Skip to that command sequence. */
                     i_index = i_next_seq;
                 }
                 else
                 {
                     /* There were other commands. */
                     msg_Warn( p_dec, "cannot recover, dropping subtitle" );
                     return VLC_EGENERIC;
                 }
            }
            else
            {
                /* We were in the last command sequence. Stop parsing by
                 * pretending we met an SPU_CMD_END command. */
                i_command = SPU_CMD_END;
                i_index++;
            }
        }
        /* */
        if( i_command == SPU_CMD_END && i_index != i_next_seq )
            break;
    }
    /* Check that the next sequence index matches the current one */
    if( i_next_seq != i_cur_seq )
    {
        msg_Err( p_dec, "index mismatch (0x%.4x != 0x%.4x)",
                 i_next_seq, i_cur_seq );
        return VLC_EGENERIC;
    }
    if( i_index > p_sys->i_spu_size )
    {
        msg_Err( p_dec, "uh-oh, we went too far (0x%.4x > 0x%.4x)",
                 i_index, p_sys->i_spu_size );
        return VLC_EGENERIC;
    }
    const int i_spu_size = p_sys->i_spu - 4;
    if( p_spu_data->pi_offset[0] < 0 || p_spu_data->pi_offset[0] >= i_spu_size ||
        p_spu_data->pi_offset[1] < 0 || p_spu_data->pi_offset[1] >= i_spu_size )
    {
        msg_Err( p_dec, "invalid offset values" );
        return VLC_EGENERIC;
    }
    if( !p_spu->i_start )
    {
        msg_Err( p_dec, "no `start display' command" );
        return VLC_EGENERIC;
    }
    if( p_spu->i_stop <= p_spu->i_start && !p_spu->b_ephemer )
    {
        /* This subtitle will live for 5 seconds or until the next subtitle */
        p_spu->i_stop = p_spu->i_start + (mtime_t)500 * 11000;
        p_spu->b_ephemer = true;
    }
    /* Get rid of padding bytes */
    if( p_sys->i_spu_size > i_index + 1 )
    {
        /* Zero or one padding byte are quite usual
         * More than one padding byte - this is very strange, but
         * we can ignore them. */
        msg_Warn( p_dec, "%i padding bytes, we usually get 0 or 1 of them",
                  p_sys->i_spu_size - i_index );
    }
    /* Successfully parsed ! */
    return VLC_SUCCESS;
}
