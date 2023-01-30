#pragma mark core interaction
static void ReportDevice(audio_output_t *p_aout, UInt32 i_id, char *name)
{
    char deviceid[10];
    sprintf(deviceid, "%i", i_id);
    aout_HotplugReport(p_aout, deviceid, name);
}
