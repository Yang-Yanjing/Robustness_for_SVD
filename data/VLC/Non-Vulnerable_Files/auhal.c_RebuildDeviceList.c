}
static void RebuildDeviceList(audio_output_t * p_aout)
{
    OSStatus            err = noErr;
    UInt32              propertySize = 0;
    AudioObjectID       *deviceIDs;
    UInt32              numberOfDevices;
    CFMutableArrayRef   currentListOfDevices;
    struct aout_sys_t   *p_sys = p_aout->sys;
    msg_Dbg(p_aout, "Rebuild device list");
    ReportDevice(p_aout, 0, _("System Sound Output Device"));
    /* setup local array */
    currentListOfDevices = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    /* Get number of devices */
    AudioObjectPropertyAddress audioDevicesAddress = { kAudioHardwarePropertyDevices, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &audioDevicesAddress, 0, NULL, &propertySize);
    if (err != noErr) {
        msg_Err(p_aout, "Could not get number of devices: [%4.4s]", (char *)&err);
        CFRelease(currentListOfDevices);
        return;
    }
    numberOfDevices = propertySize / sizeof(AudioDeviceID);
    if (numberOfDevices < 1) {
        msg_Err(p_aout, "No audio output devices found.");
        CFRelease(currentListOfDevices);
        return;
    }
    msg_Dbg(p_aout, "found %i audio device(s)", numberOfDevices);
    /* Allocate DeviceID array */
    deviceIDs = (AudioDeviceID *)calloc(numberOfDevices, sizeof(AudioDeviceID));
    if (deviceIDs == NULL) {
        CFRelease(currentListOfDevices);
        return;
    }
    /* Populate DeviceID array */
    err = AudioObjectGetPropertyData(kAudioObjectSystemObject, &audioDevicesAddress, 0, NULL, &propertySize, deviceIDs);
    if (err != noErr) {
        msg_Err(p_aout, "could not get the device IDs [%4.4s]", (char *)&err);
        CFRelease(currentListOfDevices);
        free(deviceIDs);
        return;
    }
    AudioObjectPropertyAddress deviceNameAddress = { kAudioObjectPropertyName, kAudioObjectPropertyScopeGlobal, kAudioObjectPropertyElementMaster };
    for (unsigned int i = 0; i < numberOfDevices; i++) {
        CFStringRef device_name_ref;
        char *psz_name;
        CFIndex length;
        UInt32 i_id = deviceIDs[i];
        /* Retrieve the length of the device name */
        err = AudioObjectGetPropertyDataSize(deviceIDs[i], &deviceNameAddress, 0, NULL, &propertySize);
        if (err != noErr) {
            msg_Dbg(p_aout, "failed to get name size for device %i", deviceIDs[i]);
            continue;
        }
        /* Retrieve the name of the device */
        err = AudioObjectGetPropertyData(deviceIDs[i], &deviceNameAddress, 0, NULL, &propertySize, &device_name_ref);
        if (err != noErr) {
            msg_Dbg(p_aout, "failed to get name for device %i", deviceIDs[i]);
            continue;
        }
        length = CFStringGetLength(device_name_ref);
        length++;
        psz_name = (char *)malloc(length);
        CFStringGetCString(device_name_ref, psz_name, length, kCFStringEncodingUTF8);
        msg_Dbg(p_aout, "DevID: %i DevName: %s", deviceIDs[i], psz_name);
        if (!AudioDeviceHasOutput(deviceIDs[i])) {
            msg_Dbg(p_aout, "this '%s' is INPUT only. skipping...", psz_name);
            free(psz_name);
            continue;
        }
        ReportDevice(p_aout, i_id, psz_name);
        CFNumberRef deviceNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &i_id);
        CFArrayAppendValue(currentListOfDevices, deviceNumber);
        CFRelease(deviceNumber);
        if (AudioDeviceSupportsDigital(p_aout, deviceIDs[i])) {
            msg_Dbg(p_aout, "'%s' supports digital output", psz_name);
            char *psz_encoded_name = nil;
            asprintf(&psz_encoded_name, _("%s (Encoded Output)"), psz_name);
            i_id = i_id | AOUT_VAR_SPDIF_FLAG;
            ReportDevice(p_aout, i_id, psz_encoded_name);
            deviceNumber = CFNumberCreate(kCFAllocatorDefault, kCFNumberSInt32Type, &i_id);
            CFArrayAppendValue(currentListOfDevices, deviceNumber);
            CFRelease(deviceNumber);
            free(psz_encoded_name);
        }
        // TODO: only register once for each device
        ManageAudioStreamsCallback(p_aout, deviceIDs[i], true);
        CFRelease(device_name_ref);
        free(psz_name);
    }
    vlc_mutex_lock(&p_sys->device_list_lock);
    CFIndex count = 0;
    if (p_sys->device_list)
        count = CFArrayGetCount(p_sys->device_list);
    CFRange newListSearchRange = CFRangeMake(0, CFArrayGetCount(currentListOfDevices));
    if (count > 0) {
        msg_Dbg(p_aout, "Looking for removed devices");
        CFNumberRef cfn_device_id;
        int i_device_id = 0;
        for (CFIndex x = 0; x < count; x++) {
            if (!CFArrayContainsValue(currentListOfDevices, newListSearchRange, CFArrayGetValueAtIndex(p_sys->device_list, x))) {
                cfn_device_id = CFArrayGetValueAtIndex(p_sys->device_list, x);
                if (cfn_device_id) {
                    CFNumberGetValue(cfn_device_id, kCFNumberSInt32Type, &i_device_id);
                    msg_Dbg(p_aout, "Device ID %i is not found in new array, deleting.", i_device_id);
                    ReportDevice(p_aout, i_device_id, NULL);
                }
            }
        }
    }
    if (p_sys->device_list)
        CFRelease(p_sys->device_list);
    p_sys->device_list = CFArrayCreateCopy(kCFAllocatorDefault, currentListOfDevices);
    CFRelease(currentListOfDevices);
    vlc_mutex_unlock(&p_sys->device_list_lock);
    free(deviceIDs);
}
