}
void rmff_fix_header(access_t *p_access, rmff_header_t *h) {
  unsigned int num_headers=0;
  unsigned int header_size=0;
  rmff_mdpr_t **streams;
  int num_streams=0;
  if (!h) {
    msg_Warn(p_access, "rmff_fix_header: fatal: no header given.");
    return;
  }
  if (!h->streams) {
    msg_Warn(p_access, "rmff_fix_header: warning: no MDPR chunks");
  } else
  {
    streams=h->streams;
    while (*streams)
    {
        num_streams++;
        num_headers++;
        header_size+=(*streams)->size;
        streams++;
    }
  }
  if (h->prop) {
    if (h->prop->size != 50)
    {
      msg_Dbg(p_access, "rmff_fix_header: correcting prop.size from %i to %i", h->prop->size, 50);
      h->prop->size=50;
    }
    if (h->prop->num_streams != num_streams)
    {
      msg_Dbg(p_access, "rmff_fix_header: correcting prop.num_streams from %i to %i", h->prop->num_streams, num_streams);
      h->prop->num_streams=num_streams;
    }
    num_headers++;
    header_size+=50;
  } else
    msg_Warn(p_access, "rmff_fix_header: warning: no PROP chunk.");
  if (h->cont) {
    num_headers++;
    header_size+=h->cont->size;
  } else
    msg_Warn(p_access, "rmff_fix_header: warning: no CONT chunk.");
  if (!h->data) {
    msg_Warn(p_access, "rmff_fix_header: no DATA chunk, creating one");
    h->data = calloc( 1, sizeof(rmff_data_t) );
    if( h->data )
    {
      h->data->object_id=DATA_TAG;
      h->data->object_version=0;
      h->data->size=18;
      h->data->num_packets=0;
      h->data->next_data_header=0;
    }
  }
  num_headers++;
  if (!h->fileheader) {
    msg_Warn(p_access, "rmff_fix_header: no fileheader, creating one");
    h->fileheader = calloc( 1, sizeof(rmff_fileheader_t) );
    if( h->fileheader )
    {
      h->fileheader->object_id=RMF_TAG;
      h->fileheader->size=18;
      h->fileheader->object_version=0;
      h->fileheader->file_version=0;
      h->fileheader->num_headers=num_headers+1;
    }
  }
  header_size+=h->fileheader->size;
  num_headers++;
  if(h->fileheader->num_headers != num_headers) {
    msg_Dbg(p_access, "rmff_fix_header: setting num_headers from %i to %i", h->fileheader->num_headers, num_headers);
    h->fileheader->num_headers=num_headers;
  }
  if(h->prop) {
    if (h->prop->data_offset != header_size) {
      msg_Dbg(p_access, "rmff_fix_header: setting prop.data_offset from %i to %i", h->prop->data_offset, header_size);
      h->prop->data_offset=header_size;
    }
    /* FIXME: I doubt this is right to do this here.
     * It should belong to the demux. */
    if (h->prop->num_packets == 0) {
      int p=(int)(h->prop->avg_bit_rate/8.0*(h->prop->duration/1000.0)/h->prop->avg_packet_size);
      msg_Dbg(p_access, "rmff_fix_header: assuming prop.num_packets=%i", p);
      h->prop->num_packets=p;
    }
    if (h->data->num_packets == 0) {
      msg_Dbg(p_access, "rmff_fix_header: assuming data.num_packets=%i", h->prop->num_packets);
      h->data->num_packets=h->prop->num_packets;
    }
    if (h->data->size == 18 || !h->data->size ) {
      msg_Dbg(p_access, "rmff_fix_header: assuming data.size=%i", h->prop->num_packets*h->prop->avg_packet_size);
      h->data->size+=h->prop->num_packets*h->prop->avg_packet_size;
    }
  }
}
