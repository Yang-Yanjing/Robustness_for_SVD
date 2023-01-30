/** Opens the device directory for the specified DVB adapter */
static int dvb_open_adapter (uint8_t adapter)
{
    char dir[20];
    snprintf (dir, sizeof (dir), "/dev/dvb/adapter%"PRIu8, adapter);
    return vlc_open (dir, O_SEARCH|O_DIRECTORY);
}
