const char *av_opencl_errstr(cl_int status)
{
    int i;
    for (i = 0; i < FF_ARRAY_ELEMS(opencl_err_msg); i++) {
        if (opencl_err_msg[i].err_code == status)
            return opencl_err_msg[i].err_str;
    }
    return "unknown error";
}
