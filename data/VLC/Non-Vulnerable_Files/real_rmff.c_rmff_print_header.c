}
void rmff_print_header(rmff_header_t *h) {
  rmff_mdpr_t **stream;
  if(!h) {
    printf("rmff_print_header: NULL given\n");
    return;
  }
  if(h->fileheader)
  {
    printf("\nFILE:\n");
    printf("file version      : %d\n", h->fileheader->file_version);
    printf("number of headers : %d\n", h->fileheader->num_headers);
  }
  if(h->cont)
  {
    printf("\nCONTENT:\n");
    printf("title     : %s\n", h->cont->title);
    printf("author    : %s\n", h->cont->author);
    printf("copyright : %s\n", h->cont->copyright);
    printf("comment   : %s\n", h->cont->comment);
  }
  if(h->prop)
  {
    printf("\nSTREAM PROPERTIES:\n");
    printf("bit rate (max/avg)    : %i/%i\n", h->prop->max_bit_rate, h->prop->avg_bit_rate);
    printf("packet size (max/avg) : %i/%i bytes\n", h->prop->max_packet_size, h->prop->avg_packet_size);
    printf("packets       : %i\n", h->prop->num_packets);
    printf("duration      : %i ms\n", h->prop->duration);
    printf("pre-buffer    : %i ms\n", h->prop->preroll);
    printf("index offset  : %i bytes\n", h->prop->index_offset);
    printf("data offset   : %i bytes\n", h->prop->data_offset);
    printf("media streams : %i\n", h->prop->num_streams);
    printf("flags         : ");
    if (h->prop->flags & PN_SAVE_ENABLED) printf("save_enabled ");
    if (h->prop->flags & PN_PERFECT_PLAY_ENABLED) printf("perfect_play_enabled ");
    if (h->prop->flags & PN_LIVE_BROADCAST) printf("live_broadcast ");
    printf("\n");
  }
  stream=h->streams;
  if(stream)
  {
    while (*stream)
    {
      printf("\nSTREAM %i:\n", (*stream)->stream_number);
      printf("stream name [mime type] : %s [%s]\n", (*stream)->stream_name, (*stream)->mime_type);
      printf("bit rate (max/avg)      : %i/%i\n", (*stream)->max_bit_rate, (*stream)->avg_bit_rate);
      printf("packet size (max/avg)   : %i/%i bytes\n", (*stream)->max_packet_size, (*stream)->avg_packet_size);
      printf("start time : %i\n", (*stream)->start_time);
      printf("pre-buffer : %i ms\n", (*stream)->preroll);
      printf("duration   : %i ms\n", (*stream)->duration);
      printf("type specific data:\n");
      stream++;
    }
  }
  if(h->data)
  {
    printf("\nDATA:\n");
    printf("size      : %i\n", h->data->size);
    printf("packets   : %i\n", h->data->num_packets);
    printf("next DATA : 0x%08x\n", h->data->next_data_header);
  }
}
