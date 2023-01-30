}
static int aout_OutputDeviceSet (audio_output_t *aout, const char *id)
{
    aout_OutputAssertLocked (aout);
    return (aout->device_select != NULL) ? aout->device_select (aout, id) : -1;
}
