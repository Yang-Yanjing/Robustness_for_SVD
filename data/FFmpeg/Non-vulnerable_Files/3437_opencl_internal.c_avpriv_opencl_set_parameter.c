int avpriv_opencl_set_parameter(FFOpenclParam *opencl_param, ...)
{
    int ret = 0;
    va_list arg_ptr;
    void *param;
    size_t param_size;
    cl_int status;
    if (!opencl_param->kernel) {
        av_log(opencl_param->ctx, AV_LOG_ERROR, "OpenCL kernel must be set\n");
        return AVERROR(EINVAL);
    }
    va_start(arg_ptr, opencl_param);
    do {
        param = va_arg(arg_ptr, void *);
        if (!param)
            break;
        param_size = va_arg(arg_ptr, size_t);
        if (!param_size) {
            av_log(opencl_param->ctx, AV_LOG_ERROR, "Parameter size must not be 0\n");
            ret = AVERROR(EINVAL);
            goto end;
        }
        status = clSetKernelArg(opencl_param->kernel, opencl_param->param_num, param_size, param);
        if (status != CL_SUCCESS) {
            av_log(opencl_param->ctx, AV_LOG_ERROR, "Cannot set kernel argument: %s\n", av_opencl_errstr(status));
            ret = AVERROR_EXTERNAL;
            goto end;
        }
        opencl_param->param_num++;
    } while (param && param_size);
end:
    va_end(arg_ptr);
    return ret;
}
