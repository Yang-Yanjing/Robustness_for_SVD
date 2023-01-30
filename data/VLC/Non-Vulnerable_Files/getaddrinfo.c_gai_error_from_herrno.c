static int
gai_error_from_herrno (void)
{
    switch (h_errno)
    {
        case HOST_NOT_FOUND:
            return EAI_NONAME;
        case NO_ADDRESS:
# if (NO_ADDRESS != NO_DATA)
        case NO_DATA:
# endif
            return EAI_NODATA;
        case NO_RECOVERY:
            return EAI_FAIL;
        case TRY_AGAIN:
            return EAI_AGAIN;
    }
    return EAI_SYSTEM;
}
