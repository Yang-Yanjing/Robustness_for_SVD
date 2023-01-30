int main(int argc, char **argv)
{
    if (argc < 4) {
        av_log(NULL, AV_LOG_ERROR, "Incorrect input\n");
        return 1;
    }
    av_register_all();
    if (seek_test(argv[1], argv[2], argv[3]) != 0)
        return 1;
    return 0;
}
