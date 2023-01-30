}
sdpplin_t *sdpplin_parse(access_t *p_access, char *data)
{
  sdpplin_t*        desc;
  sdpplin_stream_t* stream;
  char*             buf;
  char*             decoded;
  int               handled;
  desc = calloc( 1, sizeof(sdpplin_t) );
  if( !desc )
    return NULL;
  buf = malloc( BUFLEN );
  if( !buf )
  {
    free( desc );
    return NULL;
  }
  decoded = malloc( BUFLEN );
  if( !decoded )
  {
    free( buf );
    free( desc );
    return NULL;
  }
  desc->stream = NULL;
  while (data && *data) {
    handled=0;
    if (filter(p_access, data, "m=", &buf, BUFLEN)) {
        if ( !desc->stream ) {
            msg_Warn(p_access, "sdpplin.c: stream identifier found before stream count, skipping.");
            data = nl(data);
            continue;
        }
        stream=sdpplin_parse_stream(p_access, &data);
        msg_Dbg(p_access, "got data for stream id %u", stream->stream_id);
        if ( stream->stream_id >= desc->stream_count )
            msg_Warn(p_access, "stream id %u is greater than stream count %u\n", stream->stream_id, desc->stream_count);
        else
            desc->stream[stream->stream_id]=stream;
        continue;
    }
    if(filter(p_access, data,"a=Title:buffer;",&buf, BUFLEN)) {
      desc->title=vlc_b64_decode(buf);
      if(desc->title) {
        handled=1;
        data=nl(data);
      }
    }
    if(filter(p_access, data,"a=Author:buffer;",&buf, BUFLEN)) {
      desc->author=vlc_b64_decode(buf);
      if(desc->author) {
        handled=1;
        data=nl(data);
      }
    }
    if(filter(p_access, data,"a=Copyright:buffer;",&buf, BUFLEN)) {
      desc->copyright=vlc_b64_decode(buf);
      if(desc->copyright) {
        handled=1;
        data=nl(data);
      }
    }
    if(filter(p_access, data,"a=Abstract:buffer;",&buf, BUFLEN)) {
      desc->abstract=vlc_b64_decode(buf);
      if(desc->abstract) {
        handled=1;
        data=nl(data);
      }
    }
    if(filter(p_access, data,"a=StreamCount:integer;",&buf, BUFLEN)) {
        /* This way negative values are mapped to unfeasibly high
         * values, and will be discarded afterward
         */
        unsigned long tmp = strtoul(buf, NULL, 10);
        if ( tmp > UINT16_MAX )
            msg_Warn(p_access, "stream count out of bound: %lu\n", tmp);
        else
            desc->stream_count = tmp;
        desc->stream = malloc(sizeof(sdpplin_stream_t*)*desc->stream_count);
        handled=1;
        data=nl(data);
    }
    if(filter(p_access, data,"a=Flags:integer;",&buf, BUFLEN)) {
      desc->flags=atoi(buf);
      handled=1;
      data=nl(data);
    }
    if(!handled) {
#ifdef LOG
      int len = line_length(data);
      ;   len = len < BUFLEN ? len : BUFLEN-1;
      buf[len] = '\0';
      strncpy (buf, data, len);
      msg_Warn(p_access, "libreal: sdpplin: not handled: '%s'", buf);
#endif
      data=nl(data);
    }
  }
  free( decoded );
  free( buf );
  return desc;
}
