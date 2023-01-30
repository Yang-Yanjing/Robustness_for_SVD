}
int rmff_get_header_size(rmff_header_t *h) {
  if (!h) return 0;
  if (!h->prop) return -1;
  return h->prop->data_offset+18;
}
