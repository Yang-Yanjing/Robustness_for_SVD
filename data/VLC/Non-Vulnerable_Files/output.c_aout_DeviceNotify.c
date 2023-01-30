}
static void aout_DeviceNotify (audio_output_t *aout, const char *id)
{
    var_SetString (aout, "device", (id != NULL) ? id : "");
}
