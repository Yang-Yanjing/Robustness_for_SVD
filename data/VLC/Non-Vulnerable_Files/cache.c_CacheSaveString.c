    } while (0)
static int CacheSaveString (FILE *file, const char *str)
{
    uint16_t size = (str != NULL) ? strlen (str) : 0;
    SAVE_IMMEDIATE (size);
    if (size != 0 && fwrite (str, 1, size, file) != size)
    {
error:
        return -1;
    }
    return 0;
}
