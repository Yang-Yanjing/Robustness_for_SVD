int main(int argc, char **argv)
{
    if (argc < 2)
    {
        av_log(NULL, AV_LOG_ERROR, "Incorrect input: expected %s <name of a video file>\nNote that test works only for huffyuv, flv and mpeg4 decoders\n", argv[0]);
        return 1;
    }
    av_register_all();
    if (video_decode(argv[1]) != 0)
        return 1;
    return 0;
}
