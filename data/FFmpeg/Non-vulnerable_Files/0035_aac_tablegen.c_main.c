int main(void)
{
    ff_aac_tableinit();
    write_fileheader();
    WRITE_ARRAY("const", float, ff_aac_pow2sf_tab);
    WRITE_ARRAY("const", float, ff_aac_pow34sf_tab);
    return 0;
}
