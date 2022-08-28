int main(int argc, char **argv)
{
    if (argc < 2)
    {
        av_log(NULL, AV_LOG_ERROR, "Incorrect input\n");
        return 1;
    }
    av_register_all();
    if (video_decode_example(argv[1]) != 0)
        return 1;
    return 0;
}
