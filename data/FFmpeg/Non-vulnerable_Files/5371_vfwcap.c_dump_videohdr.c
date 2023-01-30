static void dump_videohdr(AVFormatContext *s, VIDEOHDR *vhdr)
{
#ifdef DEBUG
    av_log(s, AV_LOG_DEBUG, "VIDEOHDR\n");
    dstruct(s, vhdr, lpData, "p");
    dstruct(s, vhdr, dwBufferLength, "lu");
    dstruct(s, vhdr, dwBytesUsed, "lu");
    dstruct(s, vhdr, dwTimeCaptured, "lu");
    dstruct(s, vhdr, dwUser, "lu");
    dstruct(s, vhdr, dwFlags, "lu");
    dstruct(s, vhdr, dwReserved[0], "lu");
    dstruct(s, vhdr, dwReserved[1], "lu");
    dstruct(s, vhdr, dwReserved[2], "lu");
    dstruct(s, vhdr, dwReserved[3], "lu");
#endif
}
