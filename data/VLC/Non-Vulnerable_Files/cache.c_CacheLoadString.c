    } while (0)
static int CacheLoadString (char **p, FILE *file)
{
    char *psz = NULL;
    uint16_t size;
    LOAD_IMMEDIATE (size);
    if (size > 16384)
    {
error:
        return -1;
    }
    if (size > 0)
    {
        psz = malloc (size+1);
        if (unlikely(psz == NULL))
            goto error;
        if (fread (psz, 1, size, file) != size)
        {
            free (psz);
            goto error;
        }
        psz[size] = '\0';
    }
    *p = psz;
    return 0;
}
