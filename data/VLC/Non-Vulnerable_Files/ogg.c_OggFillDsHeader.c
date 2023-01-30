}
static int32_t OggFillDsHeader( uint8_t *p_buffer, oggds_header_t *p_oggds_header, int i_cat )
{
    int index = 0;
    p_buffer[index] = p_oggds_header->i_packet_type;
    index++;
    memcpy( &p_buffer[index], p_oggds_header->stream_type, sizeof(p_oggds_header->stream_type) );
    index += sizeof(p_oggds_header->stream_type);
    memcpy(&p_buffer[index], p_oggds_header->sub_type, sizeof(p_oggds_header->sub_type) );
    index += sizeof(p_oggds_header->sub_type);
    /* The size is filled at the end */
    uint8_t *p_isize = &p_buffer[index];
    index += 4;
    SetQWLE( &p_buffer[index], p_oggds_header->i_time_unit );
    index += 8;
    SetQWLE( &p_buffer[index], p_oggds_header->i_samples_per_unit );
    index += 8;
    SetDWLE( &p_buffer[index], p_oggds_header->i_default_len );
    index += 4;
    SetDWLE( &p_buffer[index], p_oggds_header->i_buffer_size );
    index += 4;
    SetWLE( &p_buffer[index], p_oggds_header->i_bits_per_sample );
    index += 2;
    SetWLE( &p_buffer[index], p_oggds_header->i_padding_0 );
    index += 2;
    /* audio or video */
    switch( i_cat )
    {
    case VIDEO_ES:
        SetDWLE( &p_buffer[index], p_oggds_header->header.video.i_width );
        SetDWLE( &p_buffer[index+4], p_oggds_header->header.video.i_height );
        break;
    case AUDIO_ES:
        SetWLE( &p_buffer[index], p_oggds_header->header.audio.i_channels );
        SetWLE( &p_buffer[index+2], p_oggds_header->header.audio.i_block_align );
        SetDWLE( &p_buffer[index+4], p_oggds_header->header.audio.i_avgbytespersec );
        break;
    }
    index += 8;
    SetDWLE( &p_buffer[index], p_oggds_header->i_padding_1 );
    index += 4;
    /* extra header */
    if( p_oggds_header->i_size > 0 )
    {
        memcpy( &p_buffer[index], (uint8_t *) p_oggds_header + sizeof(*p_oggds_header), p_oggds_header->i_size );
        index += p_oggds_header->i_size;
    }
    SetDWLE( p_isize, index-1 );
    return index;
}
