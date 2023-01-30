}
static int HandleSDBuffer( demux_t *p_demux, uint8_t *p_buffer,
                           unsigned int i_buffer_size )
{
    demux_sys_t *p_sys = p_demux->p_sys;
    const uint8_t *p_parser = p_buffer;
    const uint8_t *p_end = p_parser + i_buffer_size;
    const uint8_t *p_line;
    if ( p_sys->i_state != STATE_SYNC
          && p_sys->i_last_state_change < mdate() - RESYNC_TIMEOUT )
    {
        p_sys->i_state = STATE_NOSYNC;
        p_sys->i_last_state_change = mdate();
        return VLC_EGENERIC;
    }
    switch ( p_sys->i_state )
    {
    case STATE_NOSYNC:
    default:
        p_parser = FindReferenceCode( FIELD_2_VBLANK_SAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        p_sys->i_state = STATE_STARTSYNC;
        p_sys->i_last_state_change = mdate();
    case STATE_STARTSYNC:
        p_parser = FindReferenceCode( FIELD_1_VBLANK_EAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        p_sys->i_anc_size = 0;
        p_sys->i_state = STATE_ANCSYNC;
        p_sys->i_last_state_change = mdate();
    case STATE_ANCSYNC:
        p_parser = CountReference( &p_sys->i_anc_size,
                                   FIELD_1_VBLANK_SAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        p_sys->i_active_size = 0;
        p_sys->i_state = STATE_LINESYNC;
        p_sys->i_last_state_change = mdate();
    case STATE_LINESYNC:
        p_parser = CountReference( &p_sys->i_active_size,
                                   FIELD_1_VBLANK_EAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        p_sys->i_picture_size = p_sys->i_anc_size + p_sys->i_active_size;
        p_sys->i_state = STATE_ACTIVESYNC;
        p_sys->i_last_state_change = mdate();
    case STATE_ACTIVESYNC:
        p_parser = CountReference( &p_sys->i_picture_size,
                                   FIELD_1_ACTIVE_EAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        p_sys->i_line_offset = p_sys->i_picture_size
                             / (p_sys->i_anc_size + p_sys->i_active_size);
        p_sys->i_state = STATE_VBLANKSYNC;
        p_sys->i_last_state_change = mdate();
    case STATE_VBLANKSYNC:
        p_parser = CountReference( &p_sys->i_picture_size,
                                   FIELD_2_ACTIVE_EAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        p_sys->i_state = STATE_PICSYNC;
        p_sys->i_last_state_change = mdate();
    case STATE_PICSYNC:
        p_parser = CountReference( &p_sys->i_picture_size,
                                   FIELD_1_VBLANK_EAV, p_parser, p_end );
        if ( p_parser == NULL )
            break;
        if ( p_sys->i_picture_size
              % (p_sys->i_anc_size + p_sys->i_active_size) )
        {
            msg_Warn( p_demux, "wrong picture size (anc=%d active=%d total=%d offset=%d), syncing",
                 p_sys->i_anc_size, p_sys->i_active_size,
                 p_sys->i_picture_size, p_sys->i_line_offset + 1 );
            p_sys->i_state = STATE_NOSYNC;
            p_sys->i_last_state_change = mdate();
            break;
        }
        p_sys->i_nb_lines = p_sys->i_picture_size
                             / (p_sys->i_anc_size + p_sys->i_active_size);
        InitVideo( p_demux );
        msg_Dbg( p_demux,
                 "acquired sync, anc=%d active=%d lines=%d offset=%d",
                 p_sys->i_anc_size, p_sys->i_active_size,
                 p_sys->i_nb_lines, p_sys->i_line_offset + 1 );
        p_sys->i_state = STATE_SYNC;
        if( StartDecode( p_demux ) != VLC_SUCCESS )
        {
            StopDecode( p_demux );
            return VLC_ENOMEM;
        }
        p_sys->i_current_line = 0;
        p_sys->p_line_buffer = malloc( p_sys->i_anc_size
                                        + p_sys->i_active_size );
        if( !p_sys->p_line_buffer )
        {
            StopDecode( p_demux );
            return VLC_ENOMEM;
        }
        p_sys->i_line_buffer = 0;
    case STATE_SYNC:
        while ( (p_line = GetLine( p_demux, &p_parser, p_end )) != NULL )
        {
            bool b_field = p_sys->b_hd ? false :
                (p_sys->i_current_line >= p_sys->i_nb_lines / 2);
            unsigned int i_field_height = p_sys->b_hd ? p_sys->i_height :
                p_sys->i_height / 2;
            unsigned int i_field_line = b_field ?
                p_sys->i_current_line - (p_sys->i_nb_lines + 1) / 2 :
                p_sys->i_current_line;
            bool b_vbi = i_field_line < p_sys->i_line_offset ||
                i_field_line >= p_sys->i_line_offset + i_field_height;
            unsigned int anc = p_sys->i_anc_size;
            if ( p_line[0] != 0xff || p_line[1] != 0x3
                  || p_line[2] != 0x0 || p_line[3] != 0x0
                  || p_line[anc+0] != 0xff || p_line[anc+1] != 0x3
                  || p_line[anc+2] != 0x0 || p_line[anc+3] != 0x0
                  || (!b_field && b_vbi &&
                      (p_line[4] != FIELD_1_VBLANK_EAV ||
                       p_line[anc+4] != FIELD_1_VBLANK_SAV))
                  || (!b_field && !b_vbi &&
                      (p_line[4] != FIELD_1_ACTIVE_EAV ||
                       p_line[anc+4] != FIELD_1_ACTIVE_SAV))
                  || (b_field && b_vbi &&
                      (p_line[4] != FIELD_2_VBLANK_EAV ||
                       p_line[anc+4] != FIELD_2_VBLANK_SAV))
                  || (b_field && !b_vbi &&
                      (p_line[4] != FIELD_2_ACTIVE_EAV ||
                       p_line[anc+4] != FIELD_2_ACTIVE_SAV)) )
            {
                msg_Warn( p_demux, "lost sync line:%u SAV:%x EAV:%x",
                          p_sys->i_current_line + 1, p_line[4], p_line[anc+4] );
                StopDecode( p_demux );
                p_sys->i_state = STATE_NOSYNC;
                p_sys->i_last_state_change = mdate();
                break;
            }
            if ( HasAncillary( p_line + 5 ) )
            {
                /* HANC */
                unsigned int i_anc_words = (p_sys->i_anc_size - 5) * 4 / 5;
                uint16_t p_anc[i_anc_words];
                UnpackAnc( p_line + 5, p_sys->i_anc_size - 5, p_anc );
                HandleAncillary( p_demux, p_anc, i_anc_words );
            }
            if ( !b_vbi )
            {
                unsigned int i_active_field_line = i_field_line
                                                    - p_sys->i_line_offset;
                unsigned int i_active_line = b_field
                                              + i_active_field_line * 2;
                if ( !(i_active_field_line % 2) && !b_field )
                    Unpack01( p_line + anc + 5, p_sys->i_active_size - 5,
                              p_sys->p_y + p_sys->i_width * i_active_line,
                              p_sys->p_u + (p_sys->i_width / 2)
                               * (i_active_line / 2),
                              p_sys->p_v + (p_sys->i_width / 2)
                               * (i_active_line / 2) );
                else if ( !(i_active_field_line % 2) )
                    Unpack01( p_line + anc + 5, p_sys->i_active_size - 5,
                              p_sys->p_y + p_sys->i_width * i_active_line,
                              p_sys->p_u + (p_sys->i_width / 2)
                               * (i_active_line / 2 + 1),
                              p_sys->p_v + (p_sys->i_width / 2)
                               * (i_active_line / 2 + 1) );
                else if ( !b_field )
                    Unpack2( p_line + anc + 5, p_sys->i_active_size - 5,
                             p_sys->p_y + p_sys->i_width * i_active_line,
                             p_sys->p_u + (p_sys->i_width / 2)
                              * (i_active_line / 2 - 1),
                             p_sys->p_v + (p_sys->i_width / 2)
                              * (i_active_line / 2 - 1) );
                else
                    Unpack3( p_line + anc + 5, p_sys->i_active_size - 5,
                             p_sys->p_y + p_sys->i_width * i_active_line,
                             p_sys->p_u + (p_sys->i_width / 2)
                              * (i_active_line / 2),
                             p_sys->p_v + (p_sys->i_width / 2)
                              * (i_active_line / 2) );
                if ( p_sys->b_vbi && p_sys->i_height == 576
                      && p_sys->i_current_line == p_sys->i_line_offset )
                {
                    /* Line 23 is half VBI, half active */
                    UnpackVBI( p_line + anc + 5, p_sys->i_active_size - 5,
                               p_sys->p_wss_buffer );
                }
            }
            else if ( p_sys->b_vbi && p_sys->i_telx_count &&
                      i_field_line >= p_sys->i_telx_line &&
                      i_field_line < p_sys->i_telx_line
                                      + p_sys->i_telx_count )
            {
                UnpackVBI( p_line + anc + 5, p_sys->i_active_size - 5,
                    &p_sys->p_telx_buffer[(i_field_line
                        - p_sys->i_telx_line + b_field * p_sys->i_telx_count)
                        * p_sys->i_width * 2] );
            }
            else if ( b_vbi && HasAncillary( p_line + anc + 5 ) )
            {
                /* VANC */
                unsigned int i_anc_words = (p_sys->i_active_size - 5) * 4 / 5;
                uint16_t p_anc[i_anc_words];
                UnpackAnc( p_line + 5, p_sys->i_active_size - 5,
                           p_anc );
                HandleAncillary( p_demux, p_anc, i_anc_words );
            }
            p_sys->i_current_line++;
            if ( p_sys->i_current_line == p_sys->i_nb_lines )
            {
                p_sys->i_current_line %= p_sys->i_nb_lines;
                if( DecodeFrame( p_demux ) != VLC_SUCCESS )
                    return VLC_EGENERIC;
            }
        }
        break;
    }
    return VLC_SUCCESS;
}
