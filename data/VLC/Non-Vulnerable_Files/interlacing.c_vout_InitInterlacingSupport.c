}
void vout_InitInterlacingSupport(vout_thread_t *vout, bool is_interlaced)
{
    vlc_value_t val, text;
    msg_Dbg(vout, "Deinterlacing available");
    /* Create the configuration variables */
    /* */
    var_Create(vout, "deinterlace", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT | VLC_VAR_HASCHOICE);
    int deinterlace_state = var_GetInteger(vout, "deinterlace");
    deinterlace_state = VLC_CLIP(deinterlace_state, -1, 1);
    text.psz_string = _("Deinterlace");
    var_Change(vout, "deinterlace", VLC_VAR_SETTEXT, &text, NULL);
    const module_config_t *optd = config_FindConfig(VLC_OBJECT(vout), "deinterlace");
    var_Change(vout, "deinterlace", VLC_VAR_CLEARCHOICES, NULL, NULL);
    if (likely(optd != NULL))
        for (unsigned i = 0; i < optd->list_count; i++) {
            val.i_int = optd->list.i[i];
            text.psz_string = vlc_gettext(optd->list_text[i]);
            var_Change(vout, "deinterlace", VLC_VAR_ADDCHOICE, &val, &text);
        }
    var_AddCallback(vout, "deinterlace", DeinterlaceCallback, NULL);
    /* */
    var_Create(vout, "deinterlace-mode", VLC_VAR_STRING | VLC_VAR_DOINHERIT | VLC_VAR_HASCHOICE);
    char *deinterlace_mode = var_GetNonEmptyString(vout, "deinterlace-mode");
    text.psz_string = _("Deinterlace mode");
    var_Change(vout, "deinterlace-mode", VLC_VAR_SETTEXT, &text, NULL);
    const module_config_t *optm = config_FindConfig(VLC_OBJECT(vout), "deinterlace-mode");
    var_Change(vout, "deinterlace-mode", VLC_VAR_CLEARCHOICES, NULL, NULL);
    if (likely(optm != NULL))
        for (unsigned i = 0; i < optm->list_count; i++) {
             if (!DeinterlaceIsModeValid(optm->list.psz[i]))
                 continue;
             val.psz_string  = optm->list.psz[i];
             text.psz_string = vlc_gettext(optm->list_text[i]);
             var_Change(vout, "deinterlace-mode", VLC_VAR_ADDCHOICE,
                        &val, &text);
         }
    var_AddCallback(vout, "deinterlace-mode", DeinterlaceCallback, NULL);
    /* */
    var_Create(vout, "deinterlace-needed", VLC_VAR_BOOL);
    var_AddCallback(vout, "deinterlace-needed", DeinterlaceCallback, NULL);
    /* Override the initial value from filters if present */
    char *filter_mode = NULL;
    if (DeinterlaceIsPresent(vout))
        filter_mode = var_CreateGetNonEmptyString(vout, "sout-deinterlace-mode");
    if (filter_mode) {
        deinterlace_state = 1;
        free(deinterlace_mode);
        deinterlace_mode = filter_mode;
    }
    /* */
    val.psz_string = deinterlace_mode ? deinterlace_mode : optm->orig.psz;
    var_Change(vout, "deinterlace-mode", VLC_VAR_SETVALUE, &val, NULL);
    val.b_bool = is_interlaced;
    var_Change(vout, "deinterlace-needed", VLC_VAR_SETVALUE, &val, NULL);
    var_SetInteger(vout, "deinterlace", deinterlace_state);
    free(deinterlace_mode);
}
