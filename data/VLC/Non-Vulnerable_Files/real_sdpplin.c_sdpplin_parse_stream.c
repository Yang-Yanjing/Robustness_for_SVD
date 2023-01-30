}
static sdpplin_stream_t *sdpplin_parse_stream(access_t *p_access, char **data) {
  sdpplin_stream_t *desc;
  char* buf = NULL;
  unsigned char* decoded = NULL;
  int handled;
  desc = calloc( 1, sizeof(sdpplin_stream_t) );
  if( !desc )
    return NULL;
  buf = malloc( BUFLEN );
  if( !buf )
    goto error;
  decoded = malloc( BUFLEN );
  if( !decoded )
    goto error;
  if (filter(p_access, *data, "m=", &buf, BUFLEN)) {
    desc->id = strdup(buf);
  } else {
    msg_Dbg(p_access, "sdpplin: no m= found.");
    goto error;
  }
  *data=nl(*data);
  while (*data && **data && *data[0]!='m') {
    handled=0;
    if(filter(p_access, *data,"a=control:streamid=",&buf, BUFLEN)) {
        /* This way negative values are mapped to unfeasibly high
         * values, and will be discarded afterward
         */
        unsigned long tmp = strtoul(buf, NULL, 10);
        if ( tmp > UINT16_MAX )
            msg_Warn(p_access, "stream id out of bound: %lu", tmp);
        else
            desc->stream_id=tmp;
        handled=1;
        *data=nl(*data);
    }
    if(filter(p_access, *data,"a=MaxBitRate:integer;",&buf, BUFLEN)) {
      desc->max_bit_rate=atoi(buf);
      if (!desc->avg_bit_rate)
        desc->avg_bit_rate=desc->max_bit_rate;
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=MaxPacketSize:integer;",&buf, BUFLEN)) {
      desc->max_packet_size=atoi(buf);
      if (!desc->avg_packet_size)
        desc->avg_packet_size=desc->max_packet_size;
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=StartTime:integer;",&buf, BUFLEN)) {
      desc->start_time=atoi(buf);
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=Preroll:integer;",&buf, BUFLEN)) {
      desc->preroll=atoi(buf);
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=length:npt=",&buf, BUFLEN)) {
      desc->duration=(uint32_t)(atof(buf)*1000);
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=StreamName:string;",&buf, BUFLEN)) {
      desc->stream_name=strdup(buf);
      desc->stream_name_size=strlen(desc->stream_name);
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=mimetype:string;",&buf, BUFLEN)) {
      desc->mime_type=strdup(buf);
      desc->mime_type_size=strlen(desc->mime_type);
      handled=1;
      *data=nl(*data);
    }
    if(filter(p_access, *data,"a=OpaqueData:buffer;",&buf, BUFLEN)) {
      desc->mlti_data_size =
          vlc_b64_decode_binary_to_buffer(decoded, BUFLEN, buf );
      if ( desc->mlti_data_size ) {
          desc->mlti_data = malloc(desc->mlti_data_size);
          memcpy(desc->mlti_data, decoded, desc->mlti_data_size);
          handled=1;
          *data=nl(*data);
          msg_Dbg(p_access, "mlti_data_size: %i", desc->mlti_data_size);
      }
    }
    if(filter(p_access, *data,"a=ASMRuleBook:string;",&buf, BUFLEN)) {
      desc->asm_rule_book=strdup(buf);
      handled=1;
      *data=nl(*data);
    }
    if(!handled) {
#ifdef LOG
      int len = line_length(*data);
      ;   len = len < BUFLEN ? len : BUFLEN-1;
      buf[len] = '\0';
      strncpy (buf, *data, len);
      msg_Warn(p_access, "libreal: sdpplin: not handled: '%s'", buf);
#endif
      *data=nl(*data); /* always move to next line */
    }
  }
  free( buf );
  free( decoded) ;
  return desc;
error:
  free( decoded );
  free( desc );
  free( buf );
  return NULL;
}
