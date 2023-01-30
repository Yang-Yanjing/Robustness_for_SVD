/* Select non-hidden files only */
static int visible (const char *name)
{
    return name[0] != '.';
}
