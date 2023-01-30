int av_opencl_register_kernel_code(const char *kernel_code)
{
    int i, ret = init_opencl_mtx( );
    if (ret < 0)
        return ret;
    LOCK_OPENCL;
    if (opencl_ctx.kernel_code_count >= MAX_KERNEL_CODE_NUM) {
        av_log(&opencl_ctx, AV_LOG_ERROR,
               "Could not register kernel code, maximum number of registered kernel code %d already reached\n",
               MAX_KERNEL_CODE_NUM);
        ret = AVERROR(EINVAL);
        goto end;
    }
    for (i = 0; i < opencl_ctx.kernel_code_count; i++) {
        if (opencl_ctx.kernel_code[i].kernel_string == kernel_code) {
            av_log(&opencl_ctx, AV_LOG_WARNING, "Same kernel code has been registered\n");
            goto end;
        }
    }
    opencl_ctx.kernel_code[opencl_ctx.kernel_code_count].kernel_string = kernel_code;
    opencl_ctx.kernel_code[opencl_ctx.kernel_code_count].is_compiled = 0;
    opencl_ctx.kernel_code_count++;
end:
    UNLOCK_OPENCL;
    return ret;
}
