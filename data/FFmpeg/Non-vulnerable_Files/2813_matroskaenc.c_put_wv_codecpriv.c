static int put_wv_codecpriv(AVIOContext *pb, AVCodecContext *codec)
{
    if (codec->extradata && codec->extradata_size == 2)
        avio_write(pb, codec->extradata, 2);
    else
        avio_wl16(pb, 0x403); 
    return 0;
}
