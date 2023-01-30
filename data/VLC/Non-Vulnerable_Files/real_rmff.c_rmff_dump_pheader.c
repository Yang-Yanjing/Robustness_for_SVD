}
void rmff_dump_pheader(rmff_pheader_t *h, char *data) {
  data[0]=(h->object_version>>8) & 0xff;
  data[1]=h->object_version & 0xff;
  data[2]=(h->length>>8) & 0xff;
  data[3]=h->length & 0xff;
  data[4]=(h->stream_number>>8) & 0xff;
  data[5]=h->stream_number & 0xff;
  data[6]=(h->timestamp>>24) & 0xff;
  data[7]=(h->timestamp>>16) & 0xff;
  data[8]=(h->timestamp>>8) & 0xff;
  data[9]=h->timestamp & 0xff;
  data[10]=h->reserved;
  data[11]=h->flags;
}
