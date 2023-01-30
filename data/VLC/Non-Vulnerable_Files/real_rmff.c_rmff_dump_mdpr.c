}
static int rmff_dump_mdpr(rmff_mdpr_t *mdpr, uint8_t *buffer, unsigned int bufsize) {
    int s1, s2, s3;
    if (!mdpr) return 0;
    if (bufsize < RMFF_MDPRHEADER_SIZE + mdpr->type_specific_len +
            mdpr->stream_name_size + mdpr->mime_type_size)
    return -1;
    mdpr->object_id=BE_32(&mdpr->object_id);
    mdpr->size=BE_32(&mdpr->size);
    mdpr->object_version=BE_16(&mdpr->object_version);
    mdpr->stream_number=BE_16(&mdpr->stream_number);
    mdpr->max_bit_rate=BE_32(&mdpr->max_bit_rate);
    mdpr->avg_bit_rate=BE_32(&mdpr->avg_bit_rate);
    mdpr->max_packet_size=BE_32(&mdpr->max_packet_size);
    mdpr->avg_packet_size=BE_32(&mdpr->avg_packet_size);
    mdpr->start_time=BE_32(&mdpr->start_time);
    mdpr->preroll=BE_32(&mdpr->preroll);
    mdpr->duration=BE_32(&mdpr->duration);
    memcpy(buffer, mdpr, 8);
    memcpy(&buffer[8], &mdpr->object_version, 2);
    memcpy(&buffer[10], &mdpr->stream_number, 2);
    memcpy(&buffer[12], &mdpr->max_bit_rate, 28);
    memcpy(&buffer[40], &mdpr->stream_name_size, 1);
    s1=mdpr->stream_name_size;
    memcpy(&buffer[41], mdpr->stream_name, s1);
    memcpy(&buffer[41+s1], &mdpr->mime_type_size, 1);
    s2=mdpr->mime_type_size;
    memcpy(&buffer[42+s1], mdpr->mime_type, s2);
    mdpr->type_specific_len=BE_32(&mdpr->type_specific_len);
    memcpy(&buffer[42+s1+s2], &mdpr->type_specific_len, 4);
    mdpr->type_specific_len=BE_32(&mdpr->type_specific_len);
    s3=mdpr->type_specific_len;
    memcpy(&buffer[46+s1+s2], mdpr->type_specific_data, s3);
    mdpr->size=BE_32(&mdpr->size);
    mdpr->stream_number=BE_16(&mdpr->stream_number);
    mdpr->max_bit_rate=BE_32(&mdpr->max_bit_rate);
    mdpr->avg_bit_rate=BE_32(&mdpr->avg_bit_rate);
    mdpr->max_packet_size=BE_32(&mdpr->max_packet_size);
    mdpr->avg_packet_size=BE_32(&mdpr->avg_packet_size);
    mdpr->start_time=BE_32(&mdpr->start_time);
    mdpr->preroll=BE_32(&mdpr->preroll);
    mdpr->duration=BE_32(&mdpr->duration);
    mdpr->object_id=BE_32(&mdpr->object_id);
    return RMFF_MDPRHEADER_SIZE + s1 + s2 + s3;
}