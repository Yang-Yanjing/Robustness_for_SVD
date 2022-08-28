static long int read_seek_range(const char *string_with_number)
{
    long int number;
    char *end_of_string = NULL;
    number = strtol(string_with_number, &end_of_string, 10);
    if ((strlen(string_with_number) != end_of_string - string_with_number)  || (number < 0)) {
        av_log(NULL, AV_LOG_ERROR, "Incorrect input ranges of seeking\n");
        return -1;
    }
    else if ((number == LONG_MAX) || (number == LONG_MIN)) {
        if (errno == ERANGE) {
            av_log(NULL, AV_LOG_ERROR, "Incorrect input ranges of seeking\n");
            return -1;
        }
    }
    return number;
}
