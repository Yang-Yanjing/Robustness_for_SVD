 */
float aout_VolumeGet (audio_output_t *aout)
{
    return var_GetFloat (aout, "volume");
}
