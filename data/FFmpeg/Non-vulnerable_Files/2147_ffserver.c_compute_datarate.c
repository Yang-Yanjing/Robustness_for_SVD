static int compute_datarate(DataRateData *drd, int64_t count)
{
    if (cur_time == drd->time1)
        return 0;
    return ((count - drd->count1) * 1000) / (cur_time - drd->time1);
}
