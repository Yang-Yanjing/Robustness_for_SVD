 */
char *aout_DeviceGet (audio_output_t *aout)
{
    return var_GetNonEmptyString (aout, "device");
}
