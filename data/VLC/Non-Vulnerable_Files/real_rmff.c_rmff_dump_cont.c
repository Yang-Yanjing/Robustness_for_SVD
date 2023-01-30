}
static int rmff_dump_cont(rmff_cont_t *cont, uint8_t *buffer, int bufsize) {
    int p;
    if (!cont) return 0;
    if (bufsize < RMFF_CONTHEADER_SIZE + cont->title_len + cont->author_len + \
            cont->copyright_len + cont->comment_len)
        return -1;
    cont->object_id=BE_32(&cont->object_id);
    cont->size=BE_32(&cont->size);
    cont->object_version=BE_16(&cont->object_version);
    memcpy(buffer, cont, 8);
    memcpy(&buffer[8], &cont->object_version, 2);
    cont->title_len=BE_16(&cont->title_len);
    memcpy(&buffer[10], &cont->title_len, 2);
    cont->title_len=BE_16(&cont->title_len);
    memcpy(&buffer[12], cont->title, cont->title_len);
    p=12+cont->title_len;
    cont->author_len=BE_16(&cont->author_len);
    memcpy(&buffer[p], &cont->author_len, 2);
    cont->author_len=BE_16(&cont->author_len);
    memcpy(&buffer[p+2], cont->author, cont->author_len);
    p+=2+cont->author_len;
    cont->copyright_len=BE_16(&cont->copyright_len);
    memcpy(&buffer[p], &cont->copyright_len, 2);
    cont->copyright_len=BE_16(&cont->copyright_len);
    memcpy(&buffer[p+2], cont->copyright, cont->copyright_len);
    p+=2+cont->copyright_len;
    cont->comment_len=BE_16(&cont->comment_len);
    memcpy(&buffer[p], &cont->comment_len, 2);
    cont->comment_len=BE_16(&cont->comment_len);
    memcpy(&buffer[p+2], cont->comment, cont->comment_len);
    cont->size=BE_32(&cont->size);
    cont->object_version=BE_16(&cont->object_version);
    cont->object_id=BE_32(&cont->object_id);
    return RMFF_CONTHEADER_SIZE + cont->title_len + cont->author_len + \
        cont->copyright_len + cont->comment_len;
}