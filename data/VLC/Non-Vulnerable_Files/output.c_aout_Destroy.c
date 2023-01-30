 */
void aout_Destroy (audio_output_t *aout)
{
    aout_owner_t *owner = aout_owner (aout);
    aout_OutputLock (aout);
    module_unneed (aout, owner->module);
    /* Protect against late call from intf.c */
    aout->volume_set = NULL;
    aout->mute_set = NULL;
    aout->device_select = NULL;
    aout_OutputUnlock (aout);
    var_DelCallback (aout, "audio-filter", FilterCallback, NULL);
    var_DelCallback (aout, "mute", var_Copy, aout->p_parent);
    var_SetFloat (aout, "volume", -1.f);
    var_DelCallback (aout, "volume", var_Copy, aout->p_parent);
    vlc_object_release (aout);
}
