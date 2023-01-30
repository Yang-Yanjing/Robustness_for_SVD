static void mxf_read_pixel_layout(AVIOContext *pb, MXFDescriptor *descriptor)
{
    int code, value, ofs = 0;
    char layout[16] = {0}; 
    do {
        code = avio_r8(pb);
        value = avio_r8(pb);
        av_log(NULL, AV_LOG_TRACE, "pixel layout: code %#x\n", code);
        if (ofs <= 14) {
            layout[ofs++] = code;
            layout[ofs++] = value;
        } else
            break;  
    } while (code != 0); 
    ff_mxf_decode_pixel_layout(layout, &descriptor->pix_fmt);
}
