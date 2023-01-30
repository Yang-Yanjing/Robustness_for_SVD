};
static const char *fdkaac_error(AACENC_ERROR erraac)
{
    switch (erraac) {
    case AACENC_OK: return "No error";
    case AACENC_INVALID_HANDLE: return "Invalid handle";
    case AACENC_MEMORY_ERROR: return "Memory allocation error";
    case AACENC_UNSUPPORTED_PARAMETER: return "Unsupported parameter";
    case AACENC_INVALID_CONFIG: return "Invalid config";
    case AACENC_INIT_ERROR: return "Initialization error";
    case AACENC_INIT_AAC_ERROR: return "AAC library initialization error";
    case AACENC_INIT_SBR_ERROR: return "SBR library initialization error";
    case AACENC_INIT_TP_ERROR: return "Transport library initialization error";
    case AACENC_INIT_META_ERROR: return "Metadata library initialization error";
    case AACENC_ENCODE_ERROR: return "Encoding error";
    case AACENC_ENCODE_EOF: return "End of file";
    default: return "Unknown error";
    }
}
