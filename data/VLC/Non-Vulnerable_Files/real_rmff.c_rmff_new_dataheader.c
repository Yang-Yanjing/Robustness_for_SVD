}
rmff_data_t *rmff_new_dataheader(uint32_t num_packets, uint32_t next_data_header) {
  rmff_data_t *data = calloc( 1, sizeof(rmff_data_t) );
  if( !data )
    return NULL;
  data->object_id=DATA_TAG;
  data->size=18;
  data->object_version=0;
  data->num_packets=num_packets;
  data->next_data_header=next_data_header;
  return data;
}
