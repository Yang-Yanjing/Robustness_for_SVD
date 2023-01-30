 */
int aout_MuteGet (audio_output_t *aout)
{
    return var_InheritBool (aout, "mute");
}
