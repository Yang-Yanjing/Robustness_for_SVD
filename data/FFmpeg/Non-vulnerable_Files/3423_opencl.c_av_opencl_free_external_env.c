void av_opencl_free_external_env(AVOpenCLExternalEnv **ext_opencl_env)
{
    av_freep(ext_opencl_env);
}
