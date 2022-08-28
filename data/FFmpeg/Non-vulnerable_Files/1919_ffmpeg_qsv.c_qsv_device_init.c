static int qsv_device_init(InputStream *ist)
{
    int err;
    AVDictionary *dict = NULL;
    if (qsv_device) {
        err = av_dict_set(&dict, "child_device", qsv_device, 0);
        if (err < 0)
            return err;
    }
    err = av_hwdevice_ctx_create(&hw_device_ctx, AV_HWDEVICE_TYPE_QSV,
                                 ist->hwaccel_device, dict, 0);
    if (err < 0) {
        av_log(NULL, AV_LOG_ERROR, "Error creating a QSV device\n");
        goto err_out;
    }
err_out:
    if (dict)
        av_dict_free(&dict);
    return err;
}
