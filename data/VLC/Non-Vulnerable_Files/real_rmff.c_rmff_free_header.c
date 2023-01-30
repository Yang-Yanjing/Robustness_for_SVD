}
void rmff_free_header(rmff_header_t *h)
{
  if (!h) return;
  free( h->fileheader );
  free( h->prop );
  free( h->data );
  if( h->cont ) {
    free( h->cont->title );
    free( h->cont->author );
    free( h->cont->copyright );
    free( h->cont->comment );
    free( h->cont );
  }
  if (h->streams)
  {
    rmff_mdpr_t **s=h->streams;
    while(*s) {
      free((*s)->stream_name);
      free((*s)->mime_type);
      free((*s)->type_specific_data);
      free(*s);
      s++;
    }
    free(h->streams);
  }
  free(h);
}
