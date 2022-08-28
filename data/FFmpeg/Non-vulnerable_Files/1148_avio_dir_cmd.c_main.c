int main(int argc, char *argv[])
{
    const char *op = NULL;
    int ret;
    av_log_set_level(AV_LOG_DEBUG);
    if (argc < 2) {
        usage(argv[0]);
        return 1;
    }
    
    av_register_all();
    avformat_network_init();
    op = argv[1];
    if (strcmp(op, "list") == 0) {
        if (argc < 3) {
            av_log(NULL, AV_LOG_INFO, "Missing argument for list operation.\n");
            ret = AVERROR(EINVAL);
        } else {
            ret = list_op(argv[2]);
        }
    } else if (strcmp(op, "del") == 0) {
        if (argc < 3) {
            av_log(NULL, AV_LOG_INFO, "Missing argument for del operation.\n");
            ret = AVERROR(EINVAL);
        } else {
            ret = del_op(argv[2]);
        }
    } else if (strcmp(op, "move") == 0) {
        if (argc < 4) {
            av_log(NULL, AV_LOG_INFO, "Missing argument for move operation.\n");
            ret = AVERROR(EINVAL);
        } else {
            ret = move_op(argv[2], argv[3]);
        }
    } else {
        av_log(NULL, AV_LOG_INFO, "Invalid operation %s\n", op);
        ret = AVERROR(EINVAL);
    }
    avformat_network_deinit();
    return ret < 0 ? 1 : 0;
}
