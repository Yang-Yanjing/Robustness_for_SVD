/** Checks that a device is an output device */
static bool DeviceIsRender(IMMDevice *dev)
{
    void *pv;
    if (FAILED(IMMDevice_QueryInterface(dev, &IID_IMMEndpoint, &pv)))
        return false;
    IMMEndpoint *ep = pv;
    EDataFlow flow;
    if (FAILED(IMMEndpoint_GetDataFlow(ep, &flow)))
        flow = eCapture;
    IMMEndpoint_Release(ep);
    return flow == eRender;
}
