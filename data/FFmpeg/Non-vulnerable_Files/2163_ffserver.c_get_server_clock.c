static int64_t get_server_clock(HTTPContext *c)
{
    
    return (cur_time - c->start_time) * 1000;
}
