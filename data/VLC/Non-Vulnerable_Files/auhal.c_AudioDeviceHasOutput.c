 */
static int AudioDeviceHasOutput(AudioDeviceID i_dev_id)
{
    UInt32 dataSize = 0;
    OSStatus status;
    AudioObjectPropertyAddress streamsAddress = { kAudioDevicePropertyStreams, kAudioDevicePropertyScopeOutput, kAudioObjectPropertyElementMaster };
    status = AudioObjectGetPropertyDataSize(i_dev_id, &streamsAddress, 0, NULL, &dataSize);
    if (dataSize == 0 || status != noErr)
        return FALSE;
    return TRUE;
}
