void av_opencl_uninit(void)
{
    int i;
    cl_int status;
    LOCK_OPENCL;
    opencl_ctx.init_count--;
    if (opencl_ctx.is_user_created)
        goto end;
    if (opencl_ctx.init_count > 0)
        goto end;
    if (opencl_ctx.command_queue) {
        status = clReleaseCommandQueue(opencl_ctx.command_queue);
        if (status != CL_SUCCESS) {
            av_log(&opencl_ctx, AV_LOG_ERROR,
                   "Could not release OpenCL command queue: %s\n", av_opencl_errstr(status));
        }
        opencl_ctx.command_queue = NULL;
    }
    if (opencl_ctx.context) {
        status = clReleaseContext(opencl_ctx.context);
        if (status != CL_SUCCESS) {
            av_log(&opencl_ctx, AV_LOG_ERROR,
                   "Could not release OpenCL context: %s\n", av_opencl_errstr(status));
        }
        opencl_ctx.context = NULL;
    }
    for (i = 0; i < opencl_ctx.kernel_code_count; i++) {
        opencl_ctx.kernel_code[i].is_compiled = 0;
    }
    free_device_list(&opencl_ctx.device_list);
end:
    if (opencl_ctx.init_count <= 0)
        av_opt_free(&opencl_ctx); 
    UNLOCK_OPENCL;
}
