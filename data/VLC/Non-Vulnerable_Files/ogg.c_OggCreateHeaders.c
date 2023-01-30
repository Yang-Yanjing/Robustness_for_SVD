static bool OggCreateHeaders( sout_mux_t *p_mux )
{
    block_t *p_hdr = NULL;
    block_t *p_og = NULL;
    ogg_packet op;
    ogg_stream_t *p_stream;
    sout_mux_sys_t *p_sys = p_mux->p_sys;
    int i;
    if( sout_AccessOutControl( p_mux->p_access,
                               ACCESS_OUT_CAN_SEEK,
                               &p_sys->skeleton.b_create ) )
    {
        p_sys->skeleton.b_create = false;
    }
    p_sys->skeleton.b_create &= !! p_mux->i_nb_inputs;
    /* no skeleton for solo vorbis/speex/opus tracks */
    if ( p_mux->i_nb_inputs == 1 && p_mux->pp_inputs[0]->p_fmt->i_cat == AUDIO_ES )
    {
        p_sys->skeleton.b_create = false;
    }
    else
    {
        for ( int i=0; i< p_mux->i_nb_inputs; i++ )
        {
            p_stream = (ogg_stream_t*) p_mux->pp_inputs[i]->p_sys;
            if ( p_stream->p_oggds_header )
            {
                /* We don't want skeleton for OggDS */
                p_sys->skeleton.b_create = false;
                break;
            }
        }
    }
    /* Skeleton's Fishead must be the first page of the stream */
    if ( p_sys->skeleton.b_create && !p_sys->skeleton.b_head_done )
    {
        msg_Dbg( p_mux, "creating header for skeleton" );
        p_sys->skeleton.i_serial_no = p_sys->i_next_serial_no++;
        ogg_stream_init( &p_sys->skeleton.os, p_sys->skeleton.i_serial_no );
        op.bytes = 80;
        op.packet = calloc( 1, op.bytes );
        if ( op.packet == NULL ) return false;
        op.b_o_s = 1;
        op.e_o_s = 0;
        op.granulepos = 0;
        op.packetno = 0;
        OggFillSkeletonFishead( op.packet, p_mux );
        ogg_stream_packetin( &p_sys->skeleton.os, &op );
        p_og = OggStreamFlush( p_mux, &p_sys->skeleton.os, 0 );
        block_ChainAppend( &p_hdr, p_og );
        p_sys->skeleton.b_head_done = true;
        p_sys->skeleton.i_fishead_offset = p_sys->i_pos;
    }
    /* Write header for each stream. All b_o_s (beginning of stream) packets
     * must appear first in the ogg stream so we take care of them first. */
    for( int pass = 0; pass < 2; pass++ )
    {
        for( i = 0; i < p_mux->i_nb_inputs; i++ )
        {
            sout_input_t *p_input = p_mux->pp_inputs[i];
            p_stream = (ogg_stream_t*)p_input->p_sys;
            bool video = ( p_stream->i_fourcc == VLC_CODEC_THEORA || p_stream->i_fourcc == VLC_CODEC_DIRAC );
            if( ( ( pass == 0 && !video ) || ( pass == 1 && video ) ) )
                continue;
            msg_Dbg( p_mux, "creating header for %4.4s",
                     (char *)&p_stream->i_fourcc );
            ogg_stream_init( &p_stream->os, p_stream->i_serial_no );
            p_stream->b_new = false;
            p_stream->i_packet_no = 0;
            p_stream->b_started = true;
            if( p_stream->i_fourcc == VLC_CODEC_VORBIS ||
                p_stream->i_fourcc == VLC_CODEC_SPEEX ||
                p_stream->i_fourcc == VLC_CODEC_OPUS ||
                p_stream->i_fourcc == VLC_CODEC_THEORA )
            {
                /* First packet in order: vorbis/speex/theora info */
                unsigned pi_size[XIPH_MAX_HEADER_COUNT];
                void     *pp_data[XIPH_MAX_HEADER_COUNT];
                unsigned i_count;
                if( xiph_SplitHeaders( pi_size, pp_data, &i_count,
                                       p_input->p_fmt->i_extra, p_input->p_fmt->p_extra ) )
                {
                    i_count = 0;
                    pi_size[0] = 0;
                    pp_data[0] = NULL;
                }
                op.bytes  = pi_size[0];
                op.packet = pp_data[0];
                if( pi_size[0] <= 0 )
                    msg_Err( p_mux, "header data corrupted");
                op.b_o_s  = 1;
                op.e_o_s  = 0;
                op.granulepos = 0;
                op.packetno = p_stream->i_packet_no++;
                ogg_stream_packetin( &p_stream->os, &op );
                p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
                /* Get keyframe_granule_shift for theora granulepos calculation */
                if( p_stream->i_fourcc == VLC_CODEC_THEORA )
                {
                    p_stream->i_keyframe_granule_shift =
                        ( (op.packet[40] & 0x03) << 3 ) | ( (op.packet[41] & 0xe0) >> 5 );
                }
            }
            else if( p_stream->i_fourcc == VLC_CODEC_DIRAC )
            {
                op.packet = p_input->p_fmt->p_extra;
                op.bytes  = p_input->p_fmt->i_extra;
                op.b_o_s  = 1;
                op.e_o_s  = 0;
                op.granulepos = ~0;
                op.packetno = p_stream->i_packet_no++;
                ogg_stream_packetin( &p_stream->os, &op );
                p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
            }
            else if( p_stream->i_fourcc == VLC_CODEC_FLAC )
            {
                /* flac stream marker (yeah, only that in the 1st packet) */
                op.packet = (unsigned char *)"fLaC";
                op.bytes  = 4;
                op.b_o_s  = 1;
                op.e_o_s  = 0;
                op.granulepos = 0;
                op.packetno = p_stream->i_packet_no++;
                ogg_stream_packetin( &p_stream->os, &op );
                p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
            }
            else if( p_stream->i_fourcc == VLC_CODEC_VP8 )
            {
                /* VP8 Header */
                op.packet = malloc( 26 );
                if( !op.packet )
                    return false;
                op.bytes = 26;
                OggFillVP8Header( op.packet, p_input );
                op.b_o_s = 1;
                op.e_o_s = 0;
                op.granulepos = 0;
                op.packetno = p_stream->i_packet_no++;
                ogg_stream_packetin( &p_stream->os, &op );
                p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
                free( op.packet );
            }
            else if( p_stream->p_oggds_header )
            {
                /* ds header */
                op.packet = malloc( sizeof(*p_stream->p_oggds_header) + p_stream->p_oggds_header->i_size );
                if( !op.packet )
                    return false;
                op.bytes  = OggFillDsHeader( op.packet, p_stream->p_oggds_header, p_stream->i_cat );
                op.b_o_s  = 1;
                op.e_o_s  = 0;
                op.granulepos = 0;
                op.packetno = p_stream->i_packet_no++;
                ogg_stream_packetin( &p_stream->os, &op );
                p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
                free( op.packet );
            }
            block_ChainAppend( &p_hdr, p_og );
        }
    }
    /* Create fisbones if any */
    if ( p_sys->skeleton.b_create )
    {
        for( i = 0; i < p_mux->i_nb_inputs; i++ )
        {
            sout_input_t *p_input = p_mux->pp_inputs[i];
            ogg_stream_t *p_stream = (ogg_stream_t*)p_input->p_sys;
            if ( p_stream->skeleton.b_fisbone_done ) continue;
            OggGetSkeletonFisbone( &op.packet, &op.bytes, p_input, p_mux );
            if ( op.packet == NULL ) return false;
            op.b_o_s = 0;
            op.e_o_s = 0;
            op.granulepos = 0;
            op.packetno = p_sys->skeleton.i_packet_no++;
            ogg_stream_packetin( &p_sys->skeleton.os, &op );
            p_og = OggStreamFlush( p_mux, &p_sys->skeleton.os, 0 );
            block_ChainAppend( &p_hdr, p_og );
            p_stream->skeleton.b_fisbone_done = true;
        }
    }
    /* Write previous headers */
    for( p_og = p_hdr; p_og != NULL; p_og = p_og->p_next )
    {
        /* flag headers to be resent for streaming clients */
        p_og->i_flags |= BLOCK_FLAG_HEADER;
    }
    p_mux->p_sys->i_pos += sout_AccessOutWrite( p_mux->p_access, p_hdr );
    p_hdr = NULL;
    /* Create indexes if any */
    for( i = 0; i < p_mux->i_nb_inputs; i++ )
    {
        sout_input_t *p_input = p_mux->pp_inputs[i];
        ogg_stream_t *p_stream = (ogg_stream_t*)p_input->p_sys;
        /* flush stream && save offset */
        if ( p_sys->skeleton.b_create && !p_stream->skeleton.b_index_done )
        {
            if ( !p_stream->skeleton.p_index ) AllocateIndex( p_mux, p_input );
            if ( p_stream->skeleton.p_index )
            {
                msg_Dbg( p_mux, "Creating index for stream %d", p_stream->i_serial_no );
                OggGetSkeletonIndex( &op.packet, &op.bytes, p_stream );
                if ( op.packet == NULL ) return false;
                op.b_o_s = 0;
                op.e_o_s = 0;
                op.granulepos = 0;
                op.packetno = p_sys->skeleton.i_packet_no++;
                /* backup some values */
                p_stream->skeleton.i_index_offset = p_mux->p_sys->i_pos;
                p_stream->skeleton.i_index_packetno = p_sys->skeleton.os.packetno;
                p_stream->skeleton.i_index_pageno = p_sys->skeleton.os.pageno;
                ogg_stream_packetin( &p_sys->skeleton.os, &op );
                p_og = OggStreamFlush( p_mux, &p_sys->skeleton.os, 0 );
                p_mux->p_sys->i_pos += sout_AccessOutWrite( p_mux->p_access, p_og );
            }
            p_stream->skeleton.b_index_done = true;
        }
    }
    /* Take care of the non b_o_s headers */
    for( i = 0; i < p_mux->i_nb_inputs; i++ )
    {
        sout_input_t *p_input = p_mux->pp_inputs[i];
        ogg_stream_t *p_stream = (ogg_stream_t*)p_input->p_sys;
        if( p_stream->i_fourcc == VLC_CODEC_VORBIS ||
            p_stream->i_fourcc == VLC_CODEC_SPEEX ||
            p_stream->i_fourcc == VLC_CODEC_OPUS ||
            p_stream->i_fourcc == VLC_CODEC_THEORA )
        {
            unsigned pi_size[XIPH_MAX_HEADER_COUNT];
            void     *pp_data[XIPH_MAX_HEADER_COUNT];
            unsigned i_count;
            if( xiph_SplitHeaders( pi_size, pp_data, &i_count,
                                   p_input->p_fmt->i_extra, p_input->p_fmt->p_extra ) )
                i_count = 0;
            /* Special case, headers are already there in the incoming stream.
             * We need to gather them an mark them as headers. */
            for( unsigned i = 1; i < i_count; i++ )
            {
                op.bytes  = pi_size[i];
                op.packet = pp_data[i];
                if( pi_size[i] <= 0 )
                    msg_Err( p_mux, "header data corrupted");
                op.b_o_s  = 0;
                op.e_o_s  = 0;
                op.granulepos = 0;
                op.packetno = p_stream->i_packet_no++;
                ogg_stream_packetin( &p_stream->os, &op );
                msg_Dbg( p_mux, "adding non bos, secondary header" );
                if( i == i_count - 1 )
                    p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
                else
                    p_og = OggStreamPageOut( p_mux, &p_stream->os, 0 );
                if( p_og )
                    block_ChainAppend( &p_hdr, p_og );
            }
        }
        else if( p_stream->i_fourcc != VLC_CODEC_FLAC &&
                 p_stream->i_fourcc != VLC_CODEC_DIRAC )
        {
            uint8_t com[128];
            int     i_com;
            /* comment */
            com[0] = PACKET_TYPE_COMMENT;
            i_com = snprintf( (char *)(com+1), 127,
                              PACKAGE_VERSION" stream output" )
                     + 1;
            op.packet = com;
            op.bytes  = i_com;
            op.b_o_s  = 0;
            op.e_o_s  = 0;
            op.granulepos = 0;
            op.packetno = p_stream->i_packet_no++;
            ogg_stream_packetin( &p_stream->os, &op );
            p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
            block_ChainAppend( &p_hdr, p_og );
        }
        /* Special case for mp4v and flac */
        if( ( p_stream->i_fourcc == VLC_CODEC_MP4V ||
              p_stream->i_fourcc == VLC_CODEC_FLAC ) &&
            p_input->p_fmt->i_extra )
        {
            /* Send a packet with the VOL data for mp4v
             * or STREAMINFO for flac */
            msg_Dbg( p_mux, "writing extra data" );
            op.bytes  = p_input->p_fmt->i_extra;
            op.packet = p_input->p_fmt->p_extra;
            uint8_t flac_streaminfo[34 + 4];
            if( p_stream->i_fourcc == VLC_CODEC_FLAC )
            {
                if (op.bytes == 42 && !memcmp(op.packet, "fLaC", 4)) {
                    op.bytes -= 4;
                    memcpy(flac_streaminfo, op.packet + 4, 38);
                    op.packet = flac_streaminfo;
                } else if (op.bytes == 34) {
                    op.bytes += 4;
                    memcpy(flac_streaminfo + 4, op.packet, 34);
                    flac_streaminfo[0] = 0x80; /* last block, streaminfo */
                    flac_streaminfo[1] = 0;
                    flac_streaminfo[2] = 0;
                    flac_streaminfo[3] = 34; /* block size */
                    op.packet = flac_streaminfo;
                } else {
                    msg_Err(p_mux, "Invalid FLAC streaminfo (%ld bytes)",
                            op.bytes);
                }
            }
            op.b_o_s  = 0;
            op.e_o_s  = 0;
            op.granulepos = 0;
            op.packetno = p_stream->i_packet_no++;
            ogg_stream_packetin( &p_stream->os, &op );
            p_og = OggStreamFlush( p_mux, &p_stream->os, 0 );
            block_ChainAppend( &p_hdr, p_og );
        }
    }
    if ( p_sys->skeleton.b_create )
    {
        msg_Dbg( p_mux, "ending skeleton" );
        op.packet = NULL;
        op.bytes = 0;
        op.b_o_s = 0;
        op.e_o_s = 1;
        op.granulepos = 0;
        op.packetno = p_sys->skeleton.i_packet_no++;
        ogg_stream_packetin( &p_sys->skeleton.os, &op );
        p_og = OggStreamFlush( p_mux, &p_sys->skeleton.os, 0 );
        block_ChainAppend( &p_hdr, p_og );
    }
    /* set HEADER flag */
    /* flag headers to be resent for streaming clients */
    for( p_og = p_hdr; p_og != NULL; p_og = p_og->p_next )
    {
        p_og->i_flags |= BLOCK_FLAG_HEADER;
    }
    /* Write previous headers */
    p_mux->p_sys->i_pos += sout_AccessOutWrite( p_mux->p_access, p_hdr );
    return true;
}
