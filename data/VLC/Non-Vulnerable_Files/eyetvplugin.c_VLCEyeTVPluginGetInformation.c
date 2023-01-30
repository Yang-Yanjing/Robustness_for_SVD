/* called when EyeTV asks various stuff about us */
static long VLCEyeTVPluginGetInformation(VLCEyeTVPluginGlobals_t *globals, long* outAPIVersion, char* outName, char *outDescription)
{
    printf("VLC media player Plug-In: GetInfo\n");
    long result = 0;
    if( globals ) 
    {
        if( outAPIVersion )
        {
            *outAPIVersion = EYETV_PLUGIN_API_VERSION;
        }
        if( outName )
        {
            strcpy( outName, "VLC media player Plug-In");
        }
        if( outDescription )
        {
            strcpy( outDescription, "This Plug-In connects EyeTV to the VLC media player for streaming purposes.");
        }
    }
    return result;
}
