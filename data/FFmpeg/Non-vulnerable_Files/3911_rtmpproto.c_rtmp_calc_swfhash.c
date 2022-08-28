static int rtmp_calc_swfhash(URLContext *s)
{
    RTMPContext *rt = s->priv_data;
    uint8_t *in_data = NULL, *out_data = NULL, *swfdata;
    int64_t in_size, out_size;
    URLContext *stream;
    char swfhash[32];
    int swfsize;
    int ret = 0;
    
    if ((ret = ffurl_open(&stream, rt->swfverify, AVIO_FLAG_READ,
                          &s->interrupt_callback, NULL)) < 0) {
        av_log(s, AV_LOG_ERROR, "Cannot open connection %s.\n", rt->swfverify);
        goto fail;
    }
    if ((in_size = ffurl_seek(stream, 0, AVSEEK_SIZE)) < 0) {
        ret = AVERROR(EIO);
        goto fail;
    }
    if (!(in_data = av_malloc(in_size))) {
        ret = AVERROR(ENOMEM);
        goto fail;
    }
    if ((ret = ffurl_read_complete(stream, in_data, in_size)) < 0)
        goto fail;
    if (in_size < 3) {
        ret = AVERROR_INVALIDDATA;
        goto fail;
    }
    if (!memcmp(in_data, "CWS", 3)) {
        
        if (!(out_data = av_malloc(8))) {
            ret = AVERROR(ENOMEM);
            goto fail;
        }
        *in_data = 'F'; 
        memcpy(out_data, in_data, 8);
        out_size = 8;
#if CONFIG_ZLIB
        if ((ret = rtmp_uncompress_swfplayer(in_data + 8, in_size - 8,
                                             &out_data, &out_size)) < 0)
            goto fail;
#else
        av_log(s, AV_LOG_ERROR,
               "Zlib is required for decompressing the SWF player file.\n");
        ret = AVERROR(EINVAL);
        goto fail;
#endif
        swfsize = out_size;
        swfdata = out_data;
    } else {
        swfsize = in_size;
        swfdata = in_data;
    }
    
    if ((ret = ff_rtmp_calc_digest(swfdata, swfsize, 0,
                                   "Genuine Adobe Flash Player 001", 30,
                                   swfhash)) < 0)
        goto fail;
    
    av_opt_set_bin(rt, "rtmp_swfhash", swfhash, 32, 0);
    rt->swfsize = swfsize;
fail:
    av_freep(&in_data);
    av_freep(&out_data);
    ffurl_close(stream);
    return ret;
}
