static int check_keyboard_interaction(int64_t cur_time)
{
    int i, ret, key;
    static int64_t last_time;
    if (received_nb_signals)
        return AVERROR_EXIT;
    
    if(cur_time - last_time >= 100000 && !run_as_daemon){
        key =  read_key();
        last_time = cur_time;
    }else
        key = -1;
    if (key == 'q')
        return AVERROR_EXIT;
    if (key == '+') av_log_set_level(av_log_get_level()+10);
    if (key == '-') av_log_set_level(av_log_get_level()-10);
    if (key == 's') qp_hist     ^= 1;
    if (key == 'h'){
        if (do_hex_dump){
            do_hex_dump = do_pkt_dump = 0;
        } else if(do_pkt_dump){
            do_hex_dump = 1;
        } else
            do_pkt_dump = 1;
        av_log_set_level(AV_LOG_DEBUG);
    }
    if (key == 'c' || key == 'C'){
        char buf[4096], target[64], command[256], arg[256] = {0};
        double time;
        int k, n = 0;
        fprintf(stderr, "\nEnter command: <target>|all <time>|-1 <command>[ <argument>]\n");
        i = 0;
        set_tty_echo(1);
        while ((k = read_key()) != '\n' && k != '\r' && i < sizeof(buf)-1)
            if (k > 0)
                buf[i++] = k;
        buf[i] = 0;
        set_tty_echo(0);
        fprintf(stderr, "\n");
        if (k > 0 &&
            (n = sscanf(buf, "%63[^ ] %lf %255[^ ] %255[^\n]", target, &time, command, arg)) >= 3) {
            av_log(NULL, AV_LOG_DEBUG, "Processing command target:%s time:%f command:%s arg:%s",
                   target, time, command, arg);
            for (i = 0; i < nb_filtergraphs; i++) {
                FilterGraph *fg = filtergraphs[i];
                if (fg->graph) {
                    if (time < 0) {
                        ret = avfilter_graph_send_command(fg->graph, target, command, arg, buf, sizeof(buf),
                                                          key == 'c' ? AVFILTER_CMD_FLAG_ONE : 0);
                        fprintf(stderr, "Command reply for stream %d: ret:%d res:\n%s", i, ret, buf);
                    } else if (key == 'c') {
                        fprintf(stderr, "Queuing commands only on filters supporting the specific command is unsupported\n");
                        ret = AVERROR_PATCHWELCOME;
                    } else {
                        ret = avfilter_graph_queue_command(fg->graph, target, command, arg, 0, time);
                        if (ret < 0)
                            fprintf(stderr, "Queuing command failed with error %s\n", av_err2str(ret));
                    }
                }
            }
        } else {
            av_log(NULL, AV_LOG_ERROR,
                   "Parse error, at least 3 arguments were expected, "
                   "only %d given in string '%s'\n", n, buf);
        }
    }
    if (key == 'd' || key == 'D'){
        int debug=0;
        if(key == 'D') {
            debug = input_streams[0]->st->codec->debug<<1;
            if(!debug) debug = 1;
            while(debug & (FF_DEBUG_DCT_COEFF|FF_DEBUG_VIS_QP|FF_DEBUG_VIS_MB_TYPE)) 
                debug += debug;
        }else{
            char buf[32];
            int k = 0;
            i = 0;
            set_tty_echo(1);
            while ((k = read_key()) != '\n' && k != '\r' && i < sizeof(buf)-1)
                if (k > 0)
                    buf[i++] = k;
            buf[i] = 0;
            set_tty_echo(0);
            fprintf(stderr, "\n");
            if (k <= 0 || sscanf(buf, "%d", &debug)!=1)
                fprintf(stderr,"error parsing debug value\n");
        }
        for(i=0;i<nb_input_streams;i++) {
            input_streams[i]->st->codec->debug = debug;
        }
        for(i=0;i<nb_output_streams;i++) {
            OutputStream *ost = output_streams[i];
            ost->enc_ctx->debug = debug;
        }
        if(debug) av_log_set_level(AV_LOG_DEBUG);
        fprintf(stderr,"debug=%d\n", debug);
    }
    if (key == '?'){
        fprintf(stderr, "key    function\n"
                        "?      show this help\n"
                        "+      increase verbosity\n"
                        "-      decrease verbosity\n"
                        "c      Send command to first matching filter supporting it\n"
                        "C      Send/Queue command to all matching filters\n"
                        "D      cycle through available debug modes\n"
                        "h      dump packets/hex press to cycle through the 3 states\n"
                        "q      quit\n"
                        "s      Show QP histogram\n"
        );
    }
    return 0;
}
