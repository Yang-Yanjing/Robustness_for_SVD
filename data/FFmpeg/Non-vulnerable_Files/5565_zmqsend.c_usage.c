static void usage(void)
{
    printf("send message to ZMQ recipient, to use with the zmq filters\n");
    printf("usage: zmqsend [OPTIONS]\n");
    printf("\n"
           "Options:\n"
           "-b ADDRESS        set bind address\n"
           "-h                print this help\n"
           "-i INFILE         set INFILE as input file, stdin if omitted\n");
}
