int read_yesno(void)
{
    int c = getchar();
    int yesno = (av_toupper(c) == 'Y');
    while (c != '\n' && c != EOF)
        c = getchar();
    return yesno;
}
