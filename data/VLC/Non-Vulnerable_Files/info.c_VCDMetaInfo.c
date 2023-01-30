void
VCDMetaInfo( access_t *p_access, /*const*/ char *psz_mrl )
{
  vcdplayer_t    *p_vcdplayer  = (vcdplayer_t *) p_access->p_sys;
  input_thread_t *p_input = p_vcdplayer->p_input;
  vcdinfo_obj_t  *p_vcdev = p_vcdplayer->vcd;
  size_t i_entries = vcdinfo_get_num_entries(p_vcdev);
  size_t last_entry = 0;
  char *psz_cat = _("Disc");
  track_t i_track;
# define addstr(t,v) input_Control(p_input,INPUT_ADD_INFO,psz_cat,t,"%s",v)
# define addnum(t,v) input_Control(p_input,INPUT_ADD_INFO,psz_cat,t,"%d",v)
# define addhex(t,v) input_Control(p_input,INPUT_ADD_INFO,psz_cat,t,"%x",v)
  addstr(_("VCD Format"),  vcdinfo_get_format_version_str(p_vcdev));
  addstr(_("Album"),       vcdinfo_get_album_id          (p_vcdev));
  addstr(_("Application"), vcdinfo_get_application_id    (p_vcdev));
  addstr(_("Preparer"),    vcdinfo_get_preparer_id       (p_vcdev));
  addnum(_("Vol #"),       vcdinfo_get_volume_num        (p_vcdev));
  addnum(_("Vol max #"),   vcdinfo_get_volume_count      (p_vcdev));
  addstr(_("Volume Set"),  vcdinfo_get_volumeset_id      (p_vcdev));
  addstr(_("Volume"),      vcdinfo_get_volume_id         (p_vcdev));
  addstr(_("Publisher"),   vcdinfo_get_publisher_id      (p_vcdev));
  addstr(_("System Id"),   vcdinfo_get_system_id         (p_vcdev));
  addnum("LIDs",           vcdinfo_get_num_LIDs          (p_vcdev));
  addnum(_("Entries"),     vcdinfo_get_num_entries       (p_vcdev));
  addnum(_("Segments"),    vcdinfo_get_num_segments      (p_vcdev));
  addnum(_("Tracks"),      vcdinfo_get_num_tracks        (p_vcdev));
  /* Spit out track information. Could also include MSF info.
     Also build title table.
   */
  for( i_track = 1 ; i_track < p_vcdplayer->i_tracks ; i_track++ ) {
    unsigned int audio_type = vcdinfo_get_track_audio_type(p_vcdev, i_track);
    uint32_t i_secsize = vcdinfo_get_track_sect_count(p_vcdev, i_track);
    if (p_vcdplayer->b_svd) {
      addnum(_("Audio Channels"),
             vcdinfo_audio_type_num_channels(p_vcdev, audio_type) );
    }
    addnum(_("First Entry Point"), 0 );
    for ( last_entry = 0 ; last_entry < i_entries
        && vcdinfo_get_track(p_vcdev, last_entry) == i_track; last_entry++ ) ;
    addnum(_("Last Entry Point"), last_entry-1 );
    addnum(_("Track size (in sectors)"), i_secsize );
  }
  {
    lid_t i_lid;
    for( i_lid = 1 ; i_lid <= p_vcdplayer->i_lids ; i_lid++ ) {
      PsdListDescriptor_t pxd;
      if (vcdinfo_lid_get_pxd(p_vcdev, &pxd, i_lid)) {
    switch (pxd.descriptor_type) {
    case PSD_TYPE_END_LIST:
      addstr(_("type"), _("end"));
      break;
    case PSD_TYPE_PLAY_LIST:
      addstr(_("type"), _("play list"));
      addnum("items",     vcdinf_pld_get_noi(pxd.pld));
      addhex("next",      vcdinf_pld_get_next_offset(pxd.pld));
      addhex("previous",  vcdinf_pld_get_prev_offset(pxd.pld));
      addhex("return",    vcdinf_pld_get_return_offset(pxd.pld));
      addnum("wait time", vcdinf_get_wait_time(pxd.pld));
      break;
    case PSD_TYPE_SELECTION_LIST:
    case PSD_TYPE_EXT_SELECTION_LIST:
      addstr(_("type"), PSD_TYPE_SELECTION_LIST == pxd.descriptor_type
             ? _("extended selection list") : _("selection list") );
      addhex("default",          vcdinf_psd_get_default_offset(pxd.psd));
      addhex("loop count",       vcdinf_get_loop_count(pxd.psd));
      addhex("next",             vcdinf_psd_get_next_offset(pxd.psd));
      addhex("previous",         vcdinf_psd_get_prev_offset(pxd.psd));
      addhex("return",           vcdinf_psd_get_return_offset(pxd.psd));
      addhex("rejected",         vcdinf_psd_get_lid_rejected(pxd.psd));
      addhex("time-out offset",  vcdinf_get_timeout_offset(pxd.psd));
      addnum("time-out time",    vcdinf_get_timeout_time(pxd.psd));
      break;
    default:
      addstr(_("type"), _("unknown type"));
      break;
    }
      }
    }
  }
# undef  addstr
# undef  addnum
# undef  addhex
  if ( CDIO_INVALID_TRACK != i_track )
  {
    char *psz_tfmt = var_InheritString( p_access, MODULE_STRING "-title-format" );
    char *psz_name = VCDFormatStr( p_vcdplayer, psz_tfmt, psz_mrl,
                                                  &(p_vcdplayer->play_item) );
    free( psz_tfmt );
    input_Control( p_input, INPUT_SET_NAME, psz_name );
    free( psz_name );
  }
}
