int64_t av_opencl_benchmark(AVOpenCLDeviceNode *device_node, cl_platform_id platform,
                            int64_t (*benchmark)(AVOpenCLExternalEnv *ext_opencl_env))
{
    int64_t ret = 0;
    cl_int status;
    cl_context_properties cps[3];
    AVOpenCLExternalEnv *ext_opencl_env = NULL;
    ext_opencl_env = av_opencl_alloc_external_env();
    ext_opencl_env->device_id = device_node->device_id;
    ext_opencl_env->device_type = device_node->device_type;
    av_log(&opencl_ctx, AV_LOG_VERBOSE, "Performing test on OpenCL device %s\n",
           device_node->device_name);
    cps[0] = CL_CONTEXT_PLATFORM;
    cps[1] = (cl_context_properties)platform;
    cps[2] = 0;
    ext_opencl_env->context = clCreateContextFromType(cps, ext_opencl_env->device_type,
                                                      NULL, NULL, &status);
    if (status != CL_SUCCESS || !ext_opencl_env->context) {
        ret = AVERROR_EXTERNAL;
        goto end;
    }
    ext_opencl_env->command_queue = clCreateCommandQueue(ext_opencl_env->context,
                                                         ext_opencl_env->device_id, 0, &status);
    if (status != CL_SUCCESS || !ext_opencl_env->command_queue) {
        ret = AVERROR_EXTERNAL;
        goto end;
    }
    ret = benchmark(ext_opencl_env);
    if (ret < 0)
        av_log(&opencl_ctx, AV_LOG_ERROR, "Benchmark failed with OpenCL device %s\n",
               device_node->device_name);
end:
    if (ext_opencl_env->command_queue)
        clReleaseCommandQueue(ext_opencl_env->command_queue);
    if (ext_opencl_env->context)
        clReleaseContext(ext_opencl_env->context);
    av_opencl_free_external_env(&ext_opencl_env);
    return ret;
}
