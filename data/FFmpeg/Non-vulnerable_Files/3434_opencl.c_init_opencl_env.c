static int init_opencl_env(OpenclContext *opencl_ctx, AVOpenCLExternalEnv *ext_opencl_env)
{
    cl_int status;
    cl_context_properties cps[3];
    int i, ret = 0;
    AVOpenCLDeviceNode *device_node = NULL;
    if (ext_opencl_env) {
        if (opencl_ctx->is_user_created)
            return 0;
        opencl_ctx->platform_id     = ext_opencl_env->platform_id;
        opencl_ctx->is_user_created = 1;
        opencl_ctx->command_queue   = ext_opencl_env->command_queue;
        opencl_ctx->context         = ext_opencl_env->context;
        opencl_ctx->device_id       = ext_opencl_env->device_id;
        opencl_ctx->device_type     = ext_opencl_env->device_type;
    } else {
        if (!opencl_ctx->is_user_created) {
            if (!opencl_ctx->device_list.platform_num) {
                ret = get_device_list(&opencl_ctx->device_list);
                if (ret < 0) {
                    return ret;
                }
            }
            if (opencl_ctx->platform_idx >= 0) {
                if (opencl_ctx->device_list.platform_num < opencl_ctx->platform_idx + 1) {
                    av_log(opencl_ctx, AV_LOG_ERROR, "User set platform index not exist\n");
                    return AVERROR(EINVAL);
                }
                if (!opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->device_num) {
                    av_log(opencl_ctx, AV_LOG_ERROR, "No devices in user specific platform with index %d\n",
                           opencl_ctx->platform_idx);
                    return AVERROR(EINVAL);
                }
                opencl_ctx->platform_id = opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->platform_id;
            } else {
                
                for (i = 0; i < opencl_ctx->device_list.platform_num; i++) {
                    if (opencl_ctx->device_list.platform_node[i]->device_num) {
                        opencl_ctx->platform_id = opencl_ctx->device_list.platform_node[i]->platform_id;
                        opencl_ctx->platform_idx = i;
                        break;
                    }
                }
            }
            if (!opencl_ctx->platform_id) {
                av_log(opencl_ctx, AV_LOG_ERROR, "Could not get OpenCL platforms\n");
                return AVERROR_EXTERNAL;
            }
            
            if (opencl_ctx->device_idx >= 0) {
                if (opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->device_num < opencl_ctx->device_idx + 1) {
                    av_log(opencl_ctx, AV_LOG_ERROR,
                           "Could not get OpenCL device idx %d in the user set platform\n", opencl_ctx->platform_idx);
                    return AVERROR(EINVAL);
                }
            } else {
                opencl_ctx->device_idx = 0;
            }
            device_node = opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->device_node[opencl_ctx->device_idx];
            opencl_ctx->device_id = device_node->device_id;
            opencl_ctx->device_type = device_node->device_type;
            


            av_log(opencl_ctx, AV_LOG_VERBOSE, "Platform Name: %s, Device Name: %s\n",
                   opencl_ctx->device_list.platform_node[opencl_ctx->platform_idx]->platform_name,
                   device_node->device_name);
            cps[0] = CL_CONTEXT_PLATFORM;
            cps[1] = (cl_context_properties)opencl_ctx->platform_id;
            cps[2] = 0;
            opencl_ctx->context = clCreateContextFromType(cps, opencl_ctx->device_type,
                                                       NULL, NULL, &status);
            if (status != CL_SUCCESS) {
                av_log(opencl_ctx, AV_LOG_ERROR,
                       "Could not get OpenCL context from device type: %s\n", av_opencl_errstr(status));
                return AVERROR_EXTERNAL;
            }
            opencl_ctx->command_queue = clCreateCommandQueue(opencl_ctx->context, opencl_ctx->device_id,
                                                          0, &status);
            if (status != CL_SUCCESS) {
                av_log(opencl_ctx, AV_LOG_ERROR,
                       "Could not create OpenCL command queue: %s\n", av_opencl_errstr(status));
                return AVERROR_EXTERNAL;
            }
        }
    }
    return ret;
}
