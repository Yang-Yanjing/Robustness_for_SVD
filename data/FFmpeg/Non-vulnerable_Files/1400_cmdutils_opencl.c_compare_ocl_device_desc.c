static int compare_ocl_device_desc(const void *a, const void *b)
{
    const OpenCLDeviceBenchmark* va = (const OpenCLDeviceBenchmark*)a;
    const OpenCLDeviceBenchmark* vb = (const OpenCLDeviceBenchmark*)b;
    return FFDIFFSIGN(va->runtime , vb->runtime);
}
