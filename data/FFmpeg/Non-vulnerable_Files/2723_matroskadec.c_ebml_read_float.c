static int ebml_read_float(AVIOContext *pb, int size, double *num)
{
    if (size == 0)
        *num = 0;
    else if (size == 4)
        *num = av_int2float(avio_rb32(pb));
    else if (size == 8)
        *num = av_int2double(avio_rb64(pb));
    else
        return AVERROR_INVALIDDATA;
    return 0;
}
