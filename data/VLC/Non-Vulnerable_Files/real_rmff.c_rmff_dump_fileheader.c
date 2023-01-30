 */
static int rmff_dump_fileheader(rmff_fileheader_t *fileheader, uint8_t *buffer, int bufsize) {
    if (!fileheader) return 0;
    if (bufsize < RMFF_FILEHEADER_SIZE)
        return -1;
    fileheader->object_id=BE_32(&fileheader->object_id);
    fileheader->size=BE_32(&fileheader->size);
    fileheader->object_version=BE_16(&fileheader->object_version);
    fileheader->file_version=BE_32(&fileheader->file_version);
    fileheader->num_headers=BE_32(&fileheader->num_headers);
    memcpy(buffer, fileheader, 8);
    memcpy(&buffer[8], &fileheader->object_version, 2);
    memcpy(&buffer[10], &fileheader->file_version, 8);
    fileheader->size=BE_32(&fileheader->size);
    fileheader->object_version=BE_16(&fileheader->object_version);
    fileheader->file_version=BE_32(&fileheader->file_version);
    fileheader->num_headers=BE_32(&fileheader->num_headers);
    fileheader->object_id=BE_32(&fileheader->object_id);
    return RMFF_FILEHEADER_SIZE;
}