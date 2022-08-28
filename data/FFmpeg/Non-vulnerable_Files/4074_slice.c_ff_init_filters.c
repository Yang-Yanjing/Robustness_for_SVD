int ff_init_filters(SwsContext * c)
{
    int i;
    int index;
    int num_ydesc;
    int num_cdesc;
    int num_vdesc = isPlanarYUV(c->dstFormat) && !isGray(c->dstFormat) ? 2 : 1;
    int need_lum_conv = c->lumToYV12 || c->readLumPlanar || c->alpToYV12 || c->readAlpPlanar;
    int need_chr_conv = c->chrToYV12 || c->readChrPlanar;
    int need_gamma = c->is_internal_gamma;
    int srcIdx, dstIdx;
    int dst_stride = FFALIGN(c->dstW * sizeof(int16_t) + 66, 16);
    uint32_t * pal = usePal(c->srcFormat) ? c->pal_yuv : (uint32_t*)c->input_rgb2yuv_table;
    int res = 0;
    if (c->dstBpc == 16)
        dst_stride <<= 1;
    num_ydesc = need_lum_conv ? 2 : 1;
    num_cdesc = need_chr_conv ? 2 : 1;
    c->numSlice = FFMAX(num_ydesc, num_cdesc) + 2;
    c->numDesc = num_ydesc + num_cdesc + num_vdesc + (need_gamma ? 2 : 0);
    c->descIndex[0] = num_ydesc + (need_gamma ? 1 : 0);
    c->descIndex[1] = num_ydesc + num_cdesc + (need_gamma ? 1 : 0);
    c->desc = av_mallocz_array(sizeof(SwsFilterDescriptor), c->numDesc);
    if (!c->desc)
        return AVERROR(ENOMEM);
    c->slice = av_mallocz_array(sizeof(SwsSlice), c->numSlice);
    res = alloc_slice(&c->slice[0], c->srcFormat, c->srcH, c->chrSrcH, c->chrSrcHSubSample, c->chrSrcVSubSample, 0);
    if (res < 0) goto cleanup;
    for (i = 1; i < c->numSlice-2; ++i) {
        res = alloc_slice(&c->slice[i], c->srcFormat, c->vLumFilterSize + MAX_LINES_AHEAD, c->vChrFilterSize + MAX_LINES_AHEAD, c->chrSrcHSubSample, c->chrSrcVSubSample, 0);
        if (res < 0) goto cleanup;
        res = alloc_lines(&c->slice[i], FFALIGN(c->srcW*2+78, 16), c->srcW);
        if (res < 0) goto cleanup;
    }
    
    res = alloc_slice(&c->slice[i], c->srcFormat, c->vLumFilterSize + MAX_LINES_AHEAD, c->vChrFilterSize + MAX_LINES_AHEAD, c->chrDstHSubSample, c->chrDstVSubSample, 1);
    if (res < 0) goto cleanup;
    res = alloc_lines(&c->slice[i], dst_stride, c->dstW);
    if (res < 0) goto cleanup;
    fill_ones(&c->slice[i], dst_stride>>1, c->dstBpc == 16);
    
    ++i;
    res = alloc_slice(&c->slice[i], c->dstFormat, c->dstH, c->chrDstH, c->chrDstHSubSample, c->chrDstVSubSample, 0);
    if (res < 0) goto cleanup;
    index = 0;
    srcIdx = 0;
    dstIdx = 1;
    if (need_gamma) {
        res = ff_init_gamma_convert(c->desc + index, c->slice + srcIdx, c->inv_gamma);
        if (res < 0) goto cleanup;
        ++index;
    }
    if (need_lum_conv) {
        res = ff_init_desc_fmt_convert(&c->desc[index], &c->slice[srcIdx], &c->slice[dstIdx], pal);
        if (res < 0) goto cleanup;
        c->desc[index].alpha = c->alpPixBuf != 0;
        ++index;
        srcIdx = dstIdx;
    }
    dstIdx = FFMAX(num_ydesc, num_cdesc);
    res = ff_init_desc_hscale(&c->desc[index], &c->slice[srcIdx], &c->slice[dstIdx], c->hLumFilter, c->hLumFilterPos, c->hLumFilterSize, c->lumXInc);
    if (res < 0) goto cleanup;
    c->desc[index].alpha = c->alpPixBuf != 0;
    ++index;
    {
        srcIdx = 0;
        dstIdx = 1;
        if (need_chr_conv) {
            res = ff_init_desc_cfmt_convert(&c->desc[index], &c->slice[srcIdx], &c->slice[dstIdx], pal);
            if (res < 0) goto cleanup;
            ++index;
            srcIdx = dstIdx;
        }
        dstIdx = FFMAX(num_ydesc, num_cdesc);
        if (c->needs_hcscale)
            res = ff_init_desc_chscale(&c->desc[index], &c->slice[srcIdx], &c->slice[dstIdx], c->hChrFilter, c->hChrFilterPos, c->hChrFilterSize, c->chrXInc);
        else
            res = ff_init_desc_no_chr(&c->desc[index], &c->slice[srcIdx], &c->slice[dstIdx]);
        if (res < 0) goto cleanup;
    }
    ++index;
    {
        srcIdx = c->numSlice - 2;
        dstIdx = c->numSlice - 1;
        res = ff_init_vscale(c, c->desc + index, c->slice + srcIdx, c->slice + dstIdx);
        if (res < 0) goto cleanup;
    }
    ++index;
    if (need_gamma) {
        res = ff_init_gamma_convert(c->desc + index, c->slice + dstIdx, c->gamma);
        if (res < 0) goto cleanup;
    }
    return 0;
cleanup:
    ff_free_filters(c);
    return res;
}
