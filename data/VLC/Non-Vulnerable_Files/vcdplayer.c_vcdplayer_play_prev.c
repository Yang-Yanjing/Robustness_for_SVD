bool
vcdplayer_play_prev( access_t * p_access )
{
  vcdplayer_t      *p_vcdplayer= (vcdplayer_t *)p_access->p_sys;
  vcdinfo_obj_t    *p_vcdinfo  = p_vcdplayer->vcd;
  vcdinfo_itemid_t  itemid;
  dbg_print( (INPUT_DBG_CALL|INPUT_DBG_PBC),
         "current: %d" , p_vcdplayer->play_item.num);
  itemid = p_vcdplayer->play_item;
  if (vcdplayer_pbc_is_on(p_vcdplayer)) {
    vcdinfo_lid_get_pxd(p_vcdinfo, &(p_vcdplayer->pxd), p_vcdplayer->i_lid);
    switch (p_vcdplayer->pxd.descriptor_type) {
    case PSD_TYPE_SELECTION_LIST:
    case PSD_TYPE_EXT_SELECTION_LIST:
      if (p_vcdplayer->pxd.psd == NULL) return false;
      vcdplayer_update_entry(p_access,
                             vcdinf_psd_get_prev_offset(p_vcdplayer->pxd.psd),
                             &itemid.num, "prev");
      itemid.type = VCDINFO_ITEM_TYPE_LID;
      break;
    case PSD_TYPE_PLAY_LIST:
      if (p_vcdplayer->pxd.pld == NULL) return false;
      vcdplayer_update_entry(p_access,
                             vcdinf_pld_get_prev_offset(p_vcdplayer->pxd.pld),
                             &itemid.num, "prev");
      itemid.type = VCDINFO_ITEM_TYPE_LID;
      break;
    case PSD_TYPE_END_LIST:
    case PSD_TYPE_COMMAND_LIST:
      LOG_WARN( "There is no PBC 'prev' selection here" );
      return false;
    }
  } else {
    /* PBC is not on. "Prev" selection is play_item.num-1 if possible. */
    int min_entry = (VCDINFO_ITEM_TYPE_ENTRY == p_vcdplayer->play_item.type)
                    ? 0 : 1;
    if (p_vcdplayer->play_item.num > min_entry) {
      itemid.num = p_vcdplayer->play_item.num-1;
    } else {
      LOG_WARN( "At the beginning - non-PBC 'prev' not possible here" );
      return false;
    }
  }
  /** ??? p_vcdplayer->update_title(); ***/
  vcdplayer_play( p_access, itemid );
  return VLC_SUCCESS;
}