void ff_opencl_register_filter_kernel_code_all(void)
{
 #if CONFIG_OPENCL
   OPENCL_REGISTER_KERNEL_CODE(DESHAKE,     deshake);
   OPENCL_REGISTER_KERNEL_CODE(UNSHARP,     unsharp);
 #endif
}
