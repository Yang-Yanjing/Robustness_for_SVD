static inline char *double2int64str(char *buf, double v)
{
    if (isnan(v)) snprintf(buf, BUF_SIZE, "nan");
    else          snprintf(buf, BUF_SIZE, "%"PRId64, (int64_t)v);
    return buf;
}
