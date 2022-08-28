static void seek_back(AVFormatContext *s, AVIOContext *pb, int64_t pos) {
    


    if (avio_seek(pb, pos, SEEK_SET) < 0)
        av_log(s, pb->seekable ? AV_LOG_ERROR : AV_LOG_INFO, "Unable to seek back to the start\n");
}
