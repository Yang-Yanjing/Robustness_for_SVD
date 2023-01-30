}
void sdpplin_free(sdpplin_t *description) {
  int i;
  if( !description ) return;
  for( i=0; i<description->stream_count; i++ ) {
    if( description->stream[i] ) {
      free( description->stream[i]->id );
      free( description->stream[i]->bandwidth );
      free( description->stream[i]->range );
      free( description->stream[i]->length );
      free( description->stream[i]->rtpmap );
      free( description->stream[i]->mimetype );
      free( description->stream[i]->stream_name );
      free( description->stream[i]->mime_type );
      free( description->stream[i]->mlti_data );
      free( description->stream[i]->rmff_flags );
      free( description->stream[i]->asm_rule_book );
      free( description->stream[i] );
    }
  }
  if( description->stream_count )
    free( description->stream );
  free( description->owner );
  free( description->session_name );
  free( description->session_info );
  free( description->uri );
  free( description->email );
  free( description->phone );
  free( description->connection );
  free( description->bandwidth );
  free( description->title );
  free( description->author );
  free( description->copyright );
  free( description->keywords );
  free( description->asm_rule_book );
  free( description->abstract );
  free( description->range );
  free( description );
}
