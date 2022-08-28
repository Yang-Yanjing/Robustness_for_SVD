static int list_devices_for_context(AVFormatContext *s, AVDictionary *options,
                                    AVDeviceInfoList **device_list)
{
    AVDictionary *tmp = NULL;
    int ret;
    av_dict_copy(&tmp, options, 0);
    if ((ret = av_opt_set_dict2(s, &tmp, AV_OPT_SEARCH_CHILDREN)) < 0)
        goto fail;
    ret = avdevice_list_devices(s, device_list);
  fail:
    av_dict_free(&tmp);
    avformat_free_context(s);
    return ret;
}
