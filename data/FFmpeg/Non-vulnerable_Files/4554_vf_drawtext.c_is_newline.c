static inline int is_newline(uint32_t c)
{
    return c == '\n' || c == '\r' || c == '\f' || c == '\v';
}
