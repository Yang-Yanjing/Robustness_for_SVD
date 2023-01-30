}
static void aout_MuteNotify (audio_output_t *aout, bool mute)
{
    var_SetBool (aout, "mute", mute);
}
