 */
static void aout_VolumeNotify (audio_output_t *aout, float volume)
{
    var_SetFloat (aout, "volume", volume);
}
