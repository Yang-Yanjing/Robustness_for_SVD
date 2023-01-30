static char *
VCDFormatStr(vcdplayer_t *p_vcdplayer,
             const char *format_str, const char *mrl,
             const vcdinfo_itemid_t *itemid)
{
#define TEMP_STR_SIZE 256
  char        temp_str[TEMP_STR_SIZE];
  char       *tp = temp_str;
  const char *te = tp+TEMP_STR_SIZE-1;
  bool        saw_control_prefix = false;
  memset(temp_str, 0, TEMP_STR_SIZE);
  for (; *format_str && tp<te; ++format_str) {
    if (!saw_control_prefix && *format_str != '%') {
      *tp++ = *format_str;
      saw_control_prefix = false;
      continue;
    }
    switch(*format_str) {
    case '%':
      if (saw_control_prefix) {
        *tp++ = '%';
      }
      saw_control_prefix = !saw_control_prefix;
      break;
    case 'A':
      tp += snprintf(tp,te-tp,"%s",
                   vcdinfo_strip_trail(vcdinfo_get_album_id(p_vcdplayer->vcd),
                                                              MAX_ALBUM_LEN));
      break;
    case 'c':
      tp += snprintf(tp,te-tp,"%d",vcdinfo_get_volume_num(p_vcdplayer->vcd));
      break;
    case 'C':
      tp += snprintf(tp,te-tp,"%d",vcdinfo_get_volume_count(p_vcdplayer->vcd));
      break;
    case 'F':
      tp += snprintf(tp,te-tp,"%s",
                            vcdinfo_get_format_version_str(p_vcdplayer->vcd));
      break;
    case 'I':
      {
        switch (itemid->type) {
        case VCDINFO_ITEM_TYPE_TRACK:
          tp += snprintf(tp,te-tp,"%s",_("Track"));
        break;
        case VCDINFO_ITEM_TYPE_ENTRY:
          tp += snprintf(tp,te-tp,"%s",_("Entry"));
          break;
        case VCDINFO_ITEM_TYPE_SEGMENT:
          tp += snprintf(tp,te-tp,"%s",_("Segment"));
          break;
        case VCDINFO_ITEM_TYPE_LID:
          tp += snprintf(tp,te-tp,"%s",_("List ID"));
          break;
        case VCDINFO_ITEM_TYPE_SPAREID2:
          tp += snprintf(tp,te-tp,"%s",_("Navigation"));
          break;
        default:
          /* What to do? */
          ;
        }
        saw_control_prefix = false;
      }
      break;
    case 'L':
      if (vcdplayer_pbc_is_on(p_vcdplayer))
        tp += snprintf(tp,te-tp,"%s %d",_("List ID"),p_vcdplayer->i_lid);
      saw_control_prefix = false;
      break;
    case 'M':
      tp += snprintf(tp,te-tp,"%s",mrl);
      break;
    case 'N':
      tp += snprintf(tp,te-tp,"%d",itemid->num);
      break;
    case 'p':
      tp += snprintf(tp,te-tp,"%s",vcdinfo_get_preparer_id(p_vcdplayer->vcd));
      break;
    case 'P':
      tp += snprintf(tp,te-tp,"%s",vcdinfo_get_publisher_id(p_vcdplayer->vcd));
      break;
    case 'S':
      if ( VCDINFO_ITEM_TYPE_SEGMENT==itemid->type ) {
        tp += snprintf(tp,te-tp," %s",
                vcdinfo_video_type2str(p_vcdplayer->vcd, itemid->num));
      }
      saw_control_prefix = false;
      break;
    case 'T':
      tp += snprintf(tp,te-tp,"%d",p_vcdplayer->i_track);
      break;
    case 'V':
      tp += snprintf(tp,te-tp,"%s",vcdinfo_get_volumeset_id(p_vcdplayer->vcd));
      break;
    case 'v':
      tp += snprintf(tp,te-tp,"%s",vcdinfo_get_volume_id(p_vcdplayer->vcd));
      break;
    default:
      *tp++ = '%';
      if(tp<te)
        *tp++ = *format_str;
      saw_control_prefix = false;
    }
  }
  return strdup(temp_str);
}
