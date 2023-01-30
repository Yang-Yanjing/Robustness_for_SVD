void av_opencl_free_option(void)
{
    
    LOCK_OPENCL;
    av_opt_free(&opencl_ctx);
    UNLOCK_OPENCL;
}
