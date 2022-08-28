static int get_device_list(AVOpenCLDeviceList *device_list)
{
    cl_int status;
    int i, j, k, device_num, total_devices_num, ret = 0;
    int *devices_num;
    cl_platform_id *platform_ids = NULL;
    cl_device_id *device_ids = NULL;
    AVOpenCLDeviceNode *device_node = NULL;
    size_t platform_name_size = 0;
    size_t device_name_size = 0;
    status = clGetPlatformIDs(0, NULL, &device_list->platform_num);
    if (status != CL_SUCCESS) {
        av_log(&opencl_ctx, AV_LOG_ERROR,
               "Could not get OpenCL platform ids: %s\n", av_opencl_errstr(status));
        return AVERROR_EXTERNAL;
    }
    platform_ids = av_mallocz_array(device_list->platform_num, sizeof(cl_platform_id));
    if (!platform_ids)
        return AVERROR(ENOMEM);
    status = clGetPlatformIDs(device_list->platform_num, platform_ids, NULL);
    if (status != CL_SUCCESS) {
        av_log(&opencl_ctx, AV_LOG_ERROR,
                "Could not get OpenCL platform ids: %s\n", av_opencl_errstr(status));
        ret = AVERROR_EXTERNAL;
        goto end;
    }
    device_list->platform_node = av_mallocz_array(device_list->platform_num, sizeof(AVOpenCLPlatformNode *));
    if (!device_list->platform_node) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    devices_num = av_mallocz(sizeof(int) * FF_ARRAY_ELEMS(device_type));
    if (!devices_num) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    for (i = 0; i < device_list->platform_num; i++) {
        device_list->platform_node[i] = av_mallocz(sizeof(AVOpenCLPlatformNode));
        if (!device_list->platform_node[i]) {
            ret = AVERROR(ENOMEM);
            goto end;
        }
        device_list->platform_node[i]->platform_id = platform_ids[i];
        status = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_VENDOR,
                                   0, NULL, &platform_name_size);
        if (status != CL_SUCCESS) {
            av_log(&opencl_ctx, AV_LOG_WARNING,
                    "Could not get size of platform name: %s\n", av_opencl_errstr(status));
        } else {
            device_list->platform_node[i]->platform_name = av_malloc(platform_name_size * sizeof(char));
            if (!device_list->platform_node[i]->platform_name) {
                av_log(&opencl_ctx, AV_LOG_WARNING,
                        "Could not allocate memory for device name: %s\n", av_opencl_errstr(status));
            } else {
                status = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_VENDOR,
                                           platform_name_size * sizeof(char),
                                           device_list->platform_node[i]->platform_name, NULL);
                if (status != CL_SUCCESS) {
                    av_log(&opencl_ctx, AV_LOG_WARNING,
                            "Could not get platform name: %s\n", av_opencl_errstr(status));
                }
            }
        }
        total_devices_num = 0;
        for (j = 0; j < FF_ARRAY_ELEMS(device_type); j++) {
            status = clGetDeviceIDs(device_list->platform_node[i]->platform_id,
                                    device_type[j], 0, NULL, &devices_num[j]);
            total_devices_num += devices_num[j];
        }
        device_list->platform_node[i]->device_node = av_mallocz_array(total_devices_num, sizeof(AVOpenCLDeviceNode *));
        if (!device_list->platform_node[i]->device_node) {
            ret = AVERROR(ENOMEM);
            goto end;
        }
        for (j = 0; j < FF_ARRAY_ELEMS(device_type); j++) {
            if (devices_num[j]) {
                device_ids = av_mallocz_array(devices_num[j], sizeof(cl_device_id));
                if (!device_ids) {
                    ret = AVERROR(ENOMEM);
                    goto end;
                }
                status = clGetDeviceIDs(device_list->platform_node[i]->platform_id, device_type[j],
                                        devices_num[j], device_ids, NULL);
                if (status != CL_SUCCESS) {
                    av_log(&opencl_ctx, AV_LOG_WARNING,
                            "Could not get device ID: %s:\n", av_opencl_errstr(status));
                    av_freep(&device_ids);
                    continue;
                }
                for (k = 0; k < devices_num[j]; k++) {
                    device_num = device_list->platform_node[i]->device_num;
                    device_list->platform_node[i]->device_node[device_num] = av_mallocz(sizeof(AVOpenCLDeviceNode));
                    if (!device_list->platform_node[i]->device_node[device_num]) {
                        ret = AVERROR(ENOMEM);
                        goto end;
                    }
                    device_node = device_list->platform_node[i]->device_node[device_num];
                    device_node->device_id = device_ids[k];
                    device_node->device_type = device_type[j];
                    status = clGetDeviceInfo(device_node->device_id, CL_DEVICE_NAME,
                                             0, NULL, &device_name_size);
                    if (status != CL_SUCCESS) {
                        av_log(&opencl_ctx, AV_LOG_WARNING,
                                "Could not get size of device name: %s\n", av_opencl_errstr(status));
                        continue;
                    }
                    device_node->device_name = av_malloc(device_name_size * sizeof(char));
                    if (!device_node->device_name) {
                        av_log(&opencl_ctx, AV_LOG_WARNING,
                                "Could not allocate memory for device name: %s\n", av_opencl_errstr(status));
                        continue;
                    }
                    status = clGetDeviceInfo(device_node->device_id, CL_DEVICE_NAME,
                                             device_name_size * sizeof(char),
                                             device_node->device_name, NULL);
                    if (status != CL_SUCCESS) {
                        av_log(&opencl_ctx, AV_LOG_WARNING,
                                "Could not get device name: %s\n", av_opencl_errstr(status));
                        continue;
                    }
                    device_list->platform_node[i]->device_num++;
                }
                av_freep(&device_ids);
            }
        }
    }
end:
    av_freep(&platform_ids);
    av_freep(&devices_num);
    av_freep(&device_ids);
    if (ret < 0)
        free_device_list(device_list);
    return ret;
}
