int av_opencl_buffer_create(cl_mem *cl_buf, size_t cl_buf_size, int flags, void *host_ptr)
{
    cl_int status;
    *cl_buf = clCreateBuffer(opencl_ctx.context, flags, cl_buf_size, host_ptr, &status);
    if (status != CL_SUCCESS) {
        av_log(&opencl_ctx, AV_LOG_ERROR, "Could not create OpenCL buffer: %s\n", av_opencl_errstr(status));
        return AVERROR_EXTERNAL;
    }
    return 0;
}
