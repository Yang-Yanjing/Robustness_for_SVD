static int read_close(AVFormatContext* ctx)
{
    al_data *ad = ctx->priv_data;
    if (ad->device) {
        alcCaptureStop(ad->device);
        alcCaptureCloseDevice(ad->device);
    }
    return 0;
}
