static void get_word(char *buf, int buf_size, const char **pp)
{
    get_word_until_chars(buf, buf_size, SPACE_CHARS, pp);
}
