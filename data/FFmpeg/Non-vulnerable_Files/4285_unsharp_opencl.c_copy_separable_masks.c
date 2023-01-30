static int copy_separable_masks(cl_mem cl_mask_x, cl_mem cl_mask_y, int step_x, int step_y)
{
    int ret = 0;
    uint32_t *mask_x, *mask_y;
    size_t size_mask_x = sizeof(uint32_t) * (2 * step_x + 1);
    size_t size_mask_y = sizeof(uint32_t) * (2 * step_y + 1);
    mask_x = av_mallocz_array(2 * step_x + 1, sizeof(uint32_t));
    if (!mask_x) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    mask_y = av_mallocz_array(2 * step_y + 1, sizeof(uint32_t));
    if (!mask_y) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
    ret = compute_mask(step_x, mask_x);
    if (ret < 0)
        goto end;
    ret = compute_mask(step_y, mask_y);
    if (ret < 0)
        goto end;
    ret = av_opencl_buffer_write(cl_mask_x, (uint8_t *)mask_x, size_mask_x);
    ret = av_opencl_buffer_write(cl_mask_y, (uint8_t *)mask_y, size_mask_y);
end:
    av_freep(&mask_x);
    av_freep(&mask_y);
    return ret;
}
