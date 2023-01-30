}
static int rmff_dump_dataheader(rmff_data_t *data, uint8_t *buffer, int bufsize) {
  if (!data) return 0;
  if (bufsize < RMFF_DATAHEADER_SIZE)
      return -1;
  data->object_id=BE_32(&data->object_id);
  data->size=BE_32(&data->size);
  data->object_version=BE_16(&data->object_version);
  data->num_packets=BE_32(&data->num_packets);
  data->next_data_header=BE_32(&data->next_data_header);
  memcpy(buffer, data, 8);
  memcpy(&buffer[8], &data->object_version, 2);
  memcpy(&buffer[10], &data->num_packets, 8);
  data->num_packets=BE_32(&data->num_packets);
  data->next_data_header=BE_32(&data->next_data_header);
  data->size=BE_32(&data->size);
  data->object_version=BE_16(&data->object_version);
  data->object_id=BE_32(&data->object_id);
  return RMFF_DATAHEADER_SIZE;
}