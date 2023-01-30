}
static void print_desc(const char *str, unsigned margin, bool color)
{
    unsigned width = ConsoleWidth() - margin;
    if (color)
        fputs(BLUE, stdout);
    const char *word = str;
    int wordlen = 0, wordwidth = 0;
    unsigned offset = 0;
    bool newline = true;
    while (str[0])
    {
        uint32_t cp;
        size_t charlen = vlc_towc(str, &cp);
        if (unlikely(charlen == (size_t)-1))
            break;
        int charwidth = wcwidth(cp);
        if (charwidth < 0)
            charwidth = 0;
        str += charlen;
        if (iswspace(cp))
        {
            if (!newline)
            {
                putchar(' '); /* insert space */
                charwidth = 1;
            }
            fwrite(word, 1, wordlen, stdout); /* write complete word */
            word = str;
            wordlen = 0;
            wordwidth = 0;
            newline = false;
        }
        else
        {
            wordlen += charlen;
            wordwidth += charwidth;
        }
        offset += charwidth;
        if (offset >= width)
        {
            if (newline)
            {   /* overflow (word wider than line) */
                fwrite(word, 1, wordlen - charlen, stdout);
                word = str - charlen;
                wordlen = charlen;
                wordwidth = charwidth;
            }
            printf("\n%*s", margin, ""); /* new line */
            offset = wordwidth;
            newline = true;
        }
    }
    if (!newline)
        putchar(' ');
    printf(color ? "%s\n"GRAY : "%s\n", word);
}
