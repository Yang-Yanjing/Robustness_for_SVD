int main(void)
{
    aacsbr_tableinit();
    write_fileheader();
    WRITE_ARRAY_ALIGNED("static const", 32, int32_t, sbr_qmf_window_ds);
    WRITE_ARRAY_ALIGNED("static const", 32, int32_t, sbr_qmf_window_us);
    return 0;
}
