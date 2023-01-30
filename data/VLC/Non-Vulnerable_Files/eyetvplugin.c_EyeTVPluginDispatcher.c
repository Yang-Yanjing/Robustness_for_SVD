#pragma export on
long EyeTVPluginDispatcher( EyeTVPluginParams* params )
{
    long result = 0;
    switch( params->selector ) 
    {
        case kEyeTVPluginSelector_Initialize:
            result = VLCEyeTVPluginInitialize((VLCEyeTVPluginGlobals_t**)params->refCon, 
                                    params->initialize.apiVersion, params->initialize.callback);
            break;
        case kEyeTVPluginSelector_Terminate:
            result = VLCEyeTVPluginTerminate((VLCEyeTVPluginGlobals_t*)params->refCon);
            break;
        case kEyeTVPluginSelector_GetInfo:
            result = VLCEyeTVPluginGetInformation((VLCEyeTVPluginGlobals_t*)params->refCon, 
                                    params->info.pluginAPIVersion, params->info.pluginName, params->info.description);
            break;
        case kEyeTVPluginSelector_DeviceAdded:
            result = VLCEyeTVPluginDeviceAdded((VLCEyeTVPluginGlobals_t*)params->refCon, 
                                    params->deviceID, params->deviceAdded.deviceType);
            break;
        case kEyeTVPluginSelector_DeviceRemoved:
            result = VLCEyeTVPluginDeviceRemoved((VLCEyeTVPluginGlobals_t*)params->refCon, params->deviceID);
            break;
        case kEyeTVPluginSelector_PacketsArrived:
            result = VLCEyeTVPluginPacketsArrived((VLCEyeTVPluginGlobals_t*)params->refCon, params->deviceID, 
                                    params->packetsArrived.packets, params->packetsArrived.packetCount);
            break;
        case kEyeTVPluginSelector_ServiceChanged:
            result = VLCEyeTVPluginServiceChanged((VLCEyeTVPluginGlobals_t*)params->refCon, 
                                    params->deviceID, params->serviceChanged.headendID, 
                                    params->serviceChanged.transponderID, params->serviceChanged.serviceID, 
                                    params->serviceChanged.pidList, params->serviceChanged.pidCount);
            break;
    }
    return result;
}
