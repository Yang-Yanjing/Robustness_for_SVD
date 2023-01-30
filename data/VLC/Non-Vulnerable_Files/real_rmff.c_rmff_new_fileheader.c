}
rmff_fileheader_t *rmff_new_fileheader(uint32_t num_headers) {
  rmff_fileheader_t *fileheader = calloc( 1, sizeof(rmff_fileheader_t) );
  if( !fileheader )
    return NULL;
  fileheader->object_id=RMF_TAG;
  fileheader->size=18;
  fileheader->object_version=0;
  fileheader->file_version=0;
  fileheader->num_headers=num_headers;
  return fileheader;
}
