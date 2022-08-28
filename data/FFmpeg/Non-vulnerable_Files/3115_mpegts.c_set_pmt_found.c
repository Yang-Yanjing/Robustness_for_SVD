static void set_pmt_found(MpegTSContext *ts, unsigned int programid)
{
    struct Program *p = get_program(ts, programid);
    if (!p)
        return;
    p->pmt_found = 1;
}
