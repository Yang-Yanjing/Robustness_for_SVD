static int swscale(SwsContext *c, const uint8_t *src[],
                   int srcStride[], int srcSliceY,
                   int srcSliceH, uint8_t *dst[], int dstStride[])
{
    

#ifndef NEW_FILTER
    const int srcW                   = c->srcW;
#endif
    const int dstW                   = c->dstW;
    const int dstH                   = c->dstH;
#ifndef NEW_FILTER
    const int chrDstW                = c->chrDstW;
    const int chrSrcW                = c->chrSrcW;
    const int lumXInc                = c->lumXInc;
    const int chrXInc                = c->chrXInc;
#endif
    const enum AVPixelFormat dstFormat = c->dstFormat;
    const int flags                  = c->flags;
    int32_t *vLumFilterPos           = c->vLumFilterPos;
    int32_t *vChrFilterPos           = c->vChrFilterPos;
#ifndef NEW_FILTER
    int32_t *hLumFilterPos           = c->hLumFilterPos;
    int32_t *hChrFilterPos           = c->hChrFilterPos;
    int16_t *hLumFilter              = c->hLumFilter;
    int16_t *hChrFilter              = c->hChrFilter;
    int32_t *lumMmxFilter            = c->lumMmxFilter;
    int32_t *chrMmxFilter            = c->chrMmxFilter;
#endif
    const int vLumFilterSize         = c->vLumFilterSize;
    const int vChrFilterSize         = c->vChrFilterSize;
#ifndef NEW_FILTER
    const int hLumFilterSize         = c->hLumFilterSize;
    const int hChrFilterSize         = c->hChrFilterSize;
    int16_t **lumPixBuf              = c->lumPixBuf;
    int16_t **chrUPixBuf             = c->chrUPixBuf;
    int16_t **chrVPixBuf             = c->chrVPixBuf;
#endif
    int16_t **alpPixBuf              = c->alpPixBuf;
    const int vLumBufSize            = c->vLumBufSize;
    const int vChrBufSize            = c->vChrBufSize;
#ifndef NEW_FILTER
    uint8_t *formatConvBuffer        = c->formatConvBuffer;
    uint32_t *pal                    = c->pal_yuv;
#endif
    yuv2planar1_fn yuv2plane1        = c->yuv2plane1;
    yuv2planarX_fn yuv2planeX        = c->yuv2planeX;
    yuv2interleavedX_fn yuv2nv12cX   = c->yuv2nv12cX;
    yuv2packed1_fn yuv2packed1       = c->yuv2packed1;
    yuv2packed2_fn yuv2packed2       = c->yuv2packed2;
    yuv2packedX_fn yuv2packedX       = c->yuv2packedX;
    yuv2anyX_fn yuv2anyX             = c->yuv2anyX;
    const int chrSrcSliceY           =                srcSliceY >> c->chrSrcVSubSample;
    const int chrSrcSliceH           = FF_CEIL_RSHIFT(srcSliceH,   c->chrSrcVSubSample);
    int should_dither                = is9_OR_10BPS(c->srcFormat) ||
                                       is16BPS(c->srcFormat);
    int lastDstY;
    
    int dstY         = c->dstY;
    int lumBufIndex  = c->lumBufIndex;
    int chrBufIndex  = c->chrBufIndex;
    int lastInLumBuf = c->lastInLumBuf;
    int lastInChrBuf = c->lastInChrBuf;
    int perform_gamma = c->is_internal_gamma;
#ifdef NEW_FILTER
    int lumStart = 0;
    int lumEnd = c->descIndex[0];
    int chrStart = lumEnd;
    int chrEnd = c->descIndex[1];
    int vStart = chrEnd;
    int vEnd = c->numDesc;
    SwsSlice *src_slice = &c->slice[lumStart];
    SwsSlice *hout_slice = &c->slice[c->numSlice-2];
    SwsSlice *vout_slice = &c->slice[c->numSlice-1];
    SwsFilterDescriptor *desc = c->desc;
    int hasLumHoles = 1;
    int hasChrHoles = 1;
#endif
#ifndef NEW_FILTER
    if (!usePal(c->srcFormat)) {
        pal = c->input_rgb2yuv_table;
    }
#endif
    if (isPacked(c->srcFormat)) {
        src[0] =
        src[1] =
        src[2] =
        src[3] = src[0];
        srcStride[0] =
        srcStride[1] =
        srcStride[2] =
        srcStride[3] = srcStride[0];
    }
    srcStride[1] <<= c->vChrDrop;
    srcStride[2] <<= c->vChrDrop;
    DEBUG_BUFFERS("swscale() %p[%d] %p[%d] %p[%d] %p[%d] -> %p[%d] %p[%d] %p[%d] %p[%d]\n",
                  src[0], srcStride[0], src[1], srcStride[1],
                  src[2], srcStride[2], src[3], srcStride[3],
                  dst[0], dstStride[0], dst[1], dstStride[1],
                  dst[2], dstStride[2], dst[3], dstStride[3]);
    DEBUG_BUFFERS("srcSliceY: %d srcSliceH: %d dstY: %d dstH: %d\n",
                  srcSliceY, srcSliceH, dstY, dstH);
    DEBUG_BUFFERS("vLumFilterSize: %d vLumBufSize: %d vChrFilterSize: %d vChrBufSize: %d\n",
                  vLumFilterSize, vLumBufSize, vChrFilterSize, vChrBufSize);
    if (dstStride[0]&15 || dstStride[1]&15 ||
        dstStride[2]&15 || dstStride[3]&15) {
        static int warnedAlready = 0; 
        if (flags & SWS_PRINT_INFO && !warnedAlready) {
            av_log(c, AV_LOG_WARNING,
                   "Warning: dstStride is not aligned!\n"
                   "         ->cannot do aligned memory accesses anymore\n");
            warnedAlready = 1;
        }
    }
    if (   (uintptr_t)dst[0]&15 || (uintptr_t)dst[1]&15 || (uintptr_t)dst[2]&15
        || (uintptr_t)src[0]&15 || (uintptr_t)src[1]&15 || (uintptr_t)src[2]&15
        || dstStride[0]&15 || dstStride[1]&15 || dstStride[2]&15 || dstStride[3]&15
        || srcStride[0]&15 || srcStride[1]&15 || srcStride[2]&15 || srcStride[3]&15
    ) {
        static int warnedAlready=0;
        int cpu_flags = av_get_cpu_flags();
        if (HAVE_MMXEXT && (cpu_flags & AV_CPU_FLAG_SSE2) && !warnedAlready){
            av_log(c, AV_LOG_WARNING, "Warning: data is not aligned! This can lead to a speedloss\n");
            warnedAlready=1;
        }
    }
    


    if (srcSliceY == 0) {
        lumBufIndex  = -1;
        chrBufIndex  = -1;
        dstY         = 0;
        lastInLumBuf = -1;
        lastInChrBuf = -1;
    }
    if (!should_dither) {
        c->chrDither8 = c->lumDither8 = sws_pb_64;
    }
    lastDstY = dstY;
#ifdef NEW_FILTER
    ff_init_vscale_pfn(c, yuv2plane1, yuv2planeX, yuv2nv12cX,
                   yuv2packed1, yuv2packed2, yuv2packedX, yuv2anyX, c->use_mmx_vfilter);
    ff_init_slice_from_src(src_slice, (uint8_t**)src, srcStride, c->srcW,
            srcSliceY, srcSliceH, chrSrcSliceY, chrSrcSliceH);
    ff_init_slice_from_src(vout_slice, (uint8_t**)dst, dstStride, c->dstW,
            dstY, dstH, dstY >> c->chrDstVSubSample,
            FF_CEIL_RSHIFT(dstH, c->chrDstVSubSample));
    hout_slice->plane[0].sliceY = lastInLumBuf + 1;
    hout_slice->plane[1].sliceY = lastInChrBuf + 1;
    hout_slice->plane[2].sliceY = lastInChrBuf + 1;
    hout_slice->plane[3].sliceY = lastInLumBuf + 1;
    hout_slice->plane[0].sliceH =
    hout_slice->plane[1].sliceH =
    hout_slice->plane[2].sliceH =
    hout_slice->plane[3].sliceH = 0;
    hout_slice->width = dstW;
#endif
    for (; dstY < dstH; dstY++) {
        const int chrDstY = dstY >> c->chrDstVSubSample;
#ifndef NEW_FILTER
        uint8_t *dest[4]  = {
            dst[0] + dstStride[0] * dstY,
            dst[1] + dstStride[1] * chrDstY,
            dst[2] + dstStride[2] * chrDstY,
            (CONFIG_SWSCALE_ALPHA && alpPixBuf) ? dst[3] + dstStride[3] * dstY : NULL,
        };
#endif
        int use_mmx_vfilter= c->use_mmx_vfilter;
        
        const int firstLumSrcY  = FFMAX(1 - vLumFilterSize, vLumFilterPos[dstY]);
        const int firstLumSrcY2 = FFMAX(1 - vLumFilterSize, vLumFilterPos[FFMIN(dstY | ((1 << c->chrDstVSubSample) - 1), dstH - 1)]);
        
        const int firstChrSrcY  = FFMAX(1 - vChrFilterSize, vChrFilterPos[chrDstY]);
        
        int lastLumSrcY  = FFMIN(c->srcH,    firstLumSrcY  + vLumFilterSize) - 1;
        int lastLumSrcY2 = FFMIN(c->srcH,    firstLumSrcY2 + vLumFilterSize) - 1;
        int lastChrSrcY  = FFMIN(c->chrSrcH, firstChrSrcY  + vChrFilterSize) - 1;
        int enough_lines;
#ifdef NEW_FILTER
        int i;
        int posY, cPosY, firstPosY, lastPosY, firstCPosY, lastCPosY;
#endif
        
        if (firstLumSrcY > lastInLumBuf) {
#ifdef NEW_FILTER
            hasLumHoles = lastInLumBuf != firstLumSrcY - 1;
            if (hasLumHoles) {
                hout_slice->plane[0].sliceY = lastInLumBuf + 1;
                hout_slice->plane[3].sliceY = lastInLumBuf + 1;
                hout_slice->plane[0].sliceH =
                hout_slice->plane[3].sliceH = 0;
            }
#endif
            lastInLumBuf = firstLumSrcY - 1;
        }
        if (firstChrSrcY > lastInChrBuf) {
#ifdef NEW_FILTER
            hasChrHoles = lastInChrBuf != firstChrSrcY - 1;
            if (hasChrHoles) {
                hout_slice->plane[1].sliceY = lastInChrBuf + 1;
                hout_slice->plane[2].sliceY = lastInChrBuf + 1;
                hout_slice->plane[1].sliceH =
                hout_slice->plane[2].sliceH = 0;
            }
#endif
            lastInChrBuf = firstChrSrcY - 1;
        }
        av_assert0(firstLumSrcY >= lastInLumBuf - vLumBufSize + 1);
        av_assert0(firstChrSrcY >= lastInChrBuf - vChrBufSize + 1);
        DEBUG_BUFFERS("dstY: %d\n", dstY);
        DEBUG_BUFFERS("\tfirstLumSrcY: %d lastLumSrcY: %d lastInLumBuf: %d\n",
                      firstLumSrcY, lastLumSrcY, lastInLumBuf);
        DEBUG_BUFFERS("\tfirstChrSrcY: %d lastChrSrcY: %d lastInChrBuf: %d\n",
                      firstChrSrcY, lastChrSrcY, lastInChrBuf);
        
        enough_lines = lastLumSrcY2 < srcSliceY + srcSliceH &&
                       lastChrSrcY < FF_CEIL_RSHIFT(srcSliceY + srcSliceH, c->chrSrcVSubSample);
        if (!enough_lines) {
            lastLumSrcY = srcSliceY + srcSliceH - 1;
            lastChrSrcY = chrSrcSliceY + chrSrcSliceH - 1;
            DEBUG_BUFFERS("buffering slice: lastLumSrcY %d lastChrSrcY %d\n",
                          lastLumSrcY, lastChrSrcY);
        }
#ifdef NEW_FILTER
        posY = hout_slice->plane[0].sliceY + hout_slice->plane[0].sliceH;
        if (posY <= lastLumSrcY && !hasLumHoles) {
            firstPosY = FFMAX(firstLumSrcY, posY);
            lastPosY = FFMIN(lastLumSrcY + MAX_LINES_AHEAD, srcSliceY + srcSliceH - 1);
        } else {
            firstPosY = lastInLumBuf + 1;
            lastPosY = lastLumSrcY;
        }
        cPosY = hout_slice->plane[1].sliceY + hout_slice->plane[1].sliceH;
        if (cPosY <= lastChrSrcY && !hasChrHoles) {
            firstCPosY = FFMAX(firstChrSrcY, cPosY);
            lastCPosY = FFMIN(lastChrSrcY + MAX_LINES_AHEAD, FF_CEIL_RSHIFT(srcSliceY + srcSliceH, c->chrSrcVSubSample) - 1);
        } else {
            firstCPosY = lastInChrBuf + 1;
            lastCPosY = lastChrSrcY;
        }
        ff_rotate_slice(hout_slice, lastPosY, lastCPosY);
        if (posY < lastLumSrcY + 1) {
            for (i = lumStart; i < lumEnd; ++i)
                desc[i].process(c, &desc[i], firstPosY, lastPosY - firstPosY + 1);
        }
        lumBufIndex += lastLumSrcY - lastInLumBuf;
        lastInLumBuf = lastLumSrcY;
        if (cPosY < lastChrSrcY + 1) {
            for (i = chrStart; i < chrEnd; ++i)
                desc[i].process(c, &desc[i], firstCPosY, lastCPosY - firstCPosY + 1);
        }
        chrBufIndex += lastChrSrcY - lastInChrBuf;
        lastInChrBuf = lastChrSrcY;
#else
        
        while (lastInLumBuf < lastLumSrcY) {
            const uint8_t *src1[4] = {
                src[0] + (lastInLumBuf + 1 - srcSliceY) * srcStride[0],
                src[1] + (lastInLumBuf + 1 - srcSliceY) * srcStride[1],
                src[2] + (lastInLumBuf + 1 - srcSliceY) * srcStride[2],
                src[3] + (lastInLumBuf + 1 - srcSliceY) * srcStride[3],
            };
            lumBufIndex++;
            av_assert0(lumBufIndex < 2 * vLumBufSize);
            av_assert0(lastInLumBuf + 1 - srcSliceY < srcSliceH);
            av_assert0(lastInLumBuf + 1 - srcSliceY >= 0);
            if (perform_gamma)
                gamma_convert((uint8_t **)src1, srcW, c->inv_gamma);
            hyscale(c, lumPixBuf[lumBufIndex], dstW, src1, srcW, lumXInc,
                    hLumFilter, hLumFilterPos, hLumFilterSize,
                    formatConvBuffer, pal, 0);
            if (CONFIG_SWSCALE_ALPHA && alpPixBuf)
                hyscale(c, alpPixBuf[lumBufIndex], dstW, src1, srcW,
                        lumXInc, hLumFilter, hLumFilterPos, hLumFilterSize,
                        formatConvBuffer, pal, 1);
            lastInLumBuf++;
            DEBUG_BUFFERS("\t\tlumBufIndex %d: lastInLumBuf: %d\n",
                          lumBufIndex, lastInLumBuf);
        }
        while (lastInChrBuf < lastChrSrcY) {
            const uint8_t *src1[4] = {
                src[0] + (lastInChrBuf + 1 - chrSrcSliceY) * srcStride[0],
                src[1] + (lastInChrBuf + 1 - chrSrcSliceY) * srcStride[1],
                src[2] + (lastInChrBuf + 1 - chrSrcSliceY) * srcStride[2],
                src[3] + (lastInChrBuf + 1 - chrSrcSliceY) * srcStride[3],
            };
            chrBufIndex++;
            av_assert0(chrBufIndex < 2 * vChrBufSize);
            av_assert0(lastInChrBuf + 1 - chrSrcSliceY < (chrSrcSliceH));
            av_assert0(lastInChrBuf + 1 - chrSrcSliceY >= 0);
            
            if (c->needs_hcscale)
                hcscale(c, chrUPixBuf[chrBufIndex], chrVPixBuf[chrBufIndex],
                        chrDstW, src1, chrSrcW, chrXInc,
                        hChrFilter, hChrFilterPos, hChrFilterSize,
                        formatConvBuffer, pal);
            lastInChrBuf++;
            DEBUG_BUFFERS("\t\tchrBufIndex %d: lastInChrBuf: %d\n",
                          chrBufIndex, lastInChrBuf);
        }
#endif
        
        if (lumBufIndex >= vLumBufSize)
            lumBufIndex -= vLumBufSize;
        if (chrBufIndex >= vChrBufSize)
            chrBufIndex -= vChrBufSize;
        if (!enough_lines)
            break;  
#if HAVE_MMX_INLINE
        ff_updateMMXDitherTables(c, dstY, lumBufIndex, chrBufIndex,
                              lastInLumBuf, lastInChrBuf);
#endif
        if (should_dither) {
            c->chrDither8 = ff_dither_8x8_128[chrDstY & 7];
            c->lumDither8 = ff_dither_8x8_128[dstY    & 7];
        }
        if (dstY >= dstH - 2) {
            

            ff_sws_init_output_funcs(c, &yuv2plane1, &yuv2planeX, &yuv2nv12cX,
                                     &yuv2packed1, &yuv2packed2, &yuv2packedX, &yuv2anyX);
            use_mmx_vfilter= 0;
            ff_init_vscale_pfn(c, yuv2plane1, yuv2planeX, yuv2nv12cX,
                           yuv2packed1, yuv2packed2, yuv2packedX, yuv2anyX, use_mmx_vfilter);
        }
        {
#ifdef NEW_FILTER
            for (i = vStart; i < vEnd; ++i)
                desc[i].process(c, &desc[i], dstY, 1);
#else
            const int16_t **lumSrcPtr  = (const int16_t **)(void*) lumPixBuf  + lumBufIndex + firstLumSrcY - lastInLumBuf + vLumBufSize;
            const int16_t **chrUSrcPtr = (const int16_t **)(void*) chrUPixBuf + chrBufIndex + firstChrSrcY - lastInChrBuf + vChrBufSize;
            const int16_t **chrVSrcPtr = (const int16_t **)(void*) chrVPixBuf + chrBufIndex + firstChrSrcY - lastInChrBuf + vChrBufSize;
            const int16_t **alpSrcPtr  = (CONFIG_SWSCALE_ALPHA && alpPixBuf) ?
                                         (const int16_t **)(void*) alpPixBuf + lumBufIndex + firstLumSrcY - lastInLumBuf + vLumBufSize : NULL;
            int16_t *vLumFilter = c->vLumFilter;
            int16_t *vChrFilter = c->vChrFilter;
            if (isPlanarYUV(dstFormat) ||
                (isGray(dstFormat) && !isALPHA(dstFormat))) { 
                const int chrSkipMask = (1 << c->chrDstVSubSample) - 1;
                vLumFilter +=    dstY * vLumFilterSize;
                vChrFilter += chrDstY * vChrFilterSize;








                if(use_mmx_vfilter){
                    vLumFilter= (int16_t *)c->lumMmxFilter;
                    vChrFilter= (int16_t *)c->chrMmxFilter;
                }
                if (vLumFilterSize == 1) {
                    yuv2plane1(lumSrcPtr[0], dest[0], dstW, c->lumDither8, 0);
                } else {
                    yuv2planeX(vLumFilter, vLumFilterSize,
                               lumSrcPtr, dest[0],
                               dstW, c->lumDither8, 0);
                }
                if (!((dstY & chrSkipMask) || isGray(dstFormat))) {
                    if (yuv2nv12cX) {
                        yuv2nv12cX(c, vChrFilter,
                                   vChrFilterSize, chrUSrcPtr, chrVSrcPtr,
                                   dest[1], chrDstW);
                    } else if (vChrFilterSize == 1) {
                        yuv2plane1(chrUSrcPtr[0], dest[1], chrDstW, c->chrDither8, 0);
                        yuv2plane1(chrVSrcPtr[0], dest[2], chrDstW, c->chrDither8, 3);
                    } else {
                        yuv2planeX(vChrFilter,
                                   vChrFilterSize, chrUSrcPtr, dest[1],
                                   chrDstW, c->chrDither8, 0);
                        yuv2planeX(vChrFilter,
                                   vChrFilterSize, chrVSrcPtr, dest[2],
                                   chrDstW, c->chrDither8, use_mmx_vfilter ? (c->uv_offx2 >> 1) : 3);
                    }
                }
                if (CONFIG_SWSCALE_ALPHA && alpPixBuf) {
                    if(use_mmx_vfilter){
                        vLumFilter= (int16_t *)c->alpMmxFilter;
                    }
                    if (vLumFilterSize == 1) {
                        yuv2plane1(alpSrcPtr[0], dest[3], dstW,
                                   c->lumDither8, 0);
                    } else {
                        yuv2planeX(vLumFilter,
                                   vLumFilterSize, alpSrcPtr, dest[3],
                                   dstW, c->lumDither8, 0);
                    }
                }
            } else if (yuv2packedX) {
                av_assert1(lumSrcPtr  + vLumFilterSize - 1 < (const int16_t **)lumPixBuf  + vLumBufSize * 2);
                av_assert1(chrUSrcPtr + vChrFilterSize - 1 < (const int16_t **)chrUPixBuf + vChrBufSize * 2);
                if (c->yuv2packed1 && vLumFilterSize == 1 &&
                    vChrFilterSize <= 2) { 
                    int chrAlpha = vChrFilterSize == 1 ? 0 : vChrFilter[2 * dstY + 1];
                    yuv2packed1(c, *lumSrcPtr, chrUSrcPtr, chrVSrcPtr,
                                alpPixBuf ? *alpSrcPtr : NULL,
                                dest[0], dstW, chrAlpha, dstY);
                } else if (c->yuv2packed2 && vLumFilterSize == 2 &&
                           vChrFilterSize == 2) { 
                    int lumAlpha = vLumFilter[2 * dstY + 1];
                    int chrAlpha = vChrFilter[2 * dstY + 1];
                    lumMmxFilter[2] =
                    lumMmxFilter[3] = vLumFilter[2 * dstY]    * 0x10001;
                    chrMmxFilter[2] =
                    chrMmxFilter[3] = vChrFilter[2 * chrDstY] * 0x10001;
                    yuv2packed2(c, lumSrcPtr, chrUSrcPtr, chrVSrcPtr,
                                alpPixBuf ? alpSrcPtr : NULL,
                                dest[0], dstW, lumAlpha, chrAlpha, dstY);
                } else { 
                    yuv2packedX(c, vLumFilter + dstY * vLumFilterSize,
                                lumSrcPtr, vLumFilterSize,
                                vChrFilter + dstY * vChrFilterSize,
                                chrUSrcPtr, chrVSrcPtr, vChrFilterSize,
                                alpSrcPtr, dest[0], dstW, dstY);
                }
            } else {
                av_assert1(!yuv2packed1 && !yuv2packed2);
                yuv2anyX(c, vLumFilter + dstY * vLumFilterSize,
                         lumSrcPtr, vLumFilterSize,
                         vChrFilter + dstY * vChrFilterSize,
                         chrUSrcPtr, chrVSrcPtr, vChrFilterSize,
                         alpSrcPtr, dest, dstW, dstY);
            }
            if (perform_gamma)
                gamma_convert(dest, dstW, c->gamma);
#endif
        }
    }
    if (isPlanar(dstFormat) && isALPHA(dstFormat) && !alpPixBuf) {
        int length = dstW;
        int height = dstY - lastDstY;
        if (is16BPS(dstFormat) || isNBPS(dstFormat)) {
            const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(dstFormat);
            fillPlane16(dst[3], dstStride[3], length, height, lastDstY,
                    1, desc->comp[3].depth_minus1,
                    isBE(dstFormat));
        } else
            fillPlane(dst[3], dstStride[3], length, height, lastDstY, 255);
    }
#if HAVE_MMXEXT_INLINE
    if (av_get_cpu_flags() & AV_CPU_FLAG_MMXEXT)
        __asm__ volatile ("sfence" ::: "memory");
#endif
    emms_c();
    
    c->dstY         = dstY;
    c->lumBufIndex  = lumBufIndex;
    c->chrBufIndex  = chrBufIndex;
    c->lastInLumBuf = lastInLumBuf;
    c->lastInChrBuf = lastInChrBuf;
    return dstY - lastDstY;
}
