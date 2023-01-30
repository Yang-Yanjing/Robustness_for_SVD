}
static int rmff_dump_prop(rmff_prop_t *prop, uint8_t *buffer, int bufsize) {
    if (!prop) return 0;
    if (bufsize < RMFF_PROPHEADER_SIZE)
        return -1;
    prop->object_id=BE_32(&prop->object_id);
    prop->size=BE_32(&prop->size);
    prop->object_version=BE_16(&prop->object_version);
    prop->max_bit_rate=BE_32(&prop->max_bit_rate);
    prop->avg_bit_rate=BE_32(&prop->avg_bit_rate);
    prop->max_packet_size=BE_32(&prop->max_packet_size);
    prop->avg_packet_size=BE_32(&prop->avg_packet_size);
    prop->num_packets=BE_32(&prop->num_packets);
    prop->duration=BE_32(&prop->duration);
    prop->preroll=BE_32(&prop->preroll);
    prop->index_offset=BE_32(&prop->index_offset);
    prop->data_offset=BE_32(&prop->data_offset);
    prop->num_streams=BE_16(&prop->num_streams);
    prop->flags=BE_16(&prop->flags);
    memcpy(buffer, prop, 8);
    memcpy(&buffer[8], &prop->object_version, 2);
    memcpy(&buffer[10], &prop->max_bit_rate, 36);
    memcpy(&buffer[46], &prop->num_streams, 2);
    memcpy(&buffer[48], &prop->flags, 2);
    prop->size=BE_32(&prop->size);
    prop->object_version=BE_16(&prop->object_version);
    prop->max_bit_rate=BE_32(&prop->max_bit_rate);
    prop->avg_bit_rate=BE_32(&prop->avg_bit_rate);
    prop->max_packet_size=BE_32(&prop->max_packet_size);
    prop->avg_packet_size=BE_32(&prop->avg_packet_size);
    prop->num_packets=BE_32(&prop->num_packets);
    prop->duration=BE_32(&prop->duration);
    prop->preroll=BE_32(&prop->preroll);
    prop->index_offset=BE_32(&prop->index_offset);
    prop->data_offset=BE_32(&prop->data_offset);
    prop->num_streams=BE_16(&prop->num_streams);
    prop->flags=BE_16(&prop->flags);
    prop->object_id=BE_32(&prop->object_id);
    return RMFF_PROPHEADER_SIZE;
}