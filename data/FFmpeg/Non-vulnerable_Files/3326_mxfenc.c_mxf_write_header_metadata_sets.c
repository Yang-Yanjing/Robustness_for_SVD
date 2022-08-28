static int mxf_write_header_metadata_sets(AVFormatContext *s)
{
    const char *material_package_name = NULL;
    const char *file_package_name = NULL;
    AVDictionaryEntry *entry = NULL;
    AVStream *st = NULL;
    int i;
    if (entry = av_dict_get(s->metadata, "material_package_name", NULL, 0))
       material_package_name = entry->value;
    if (entry = av_dict_get(s->metadata, "file_package_name", NULL, 0)) {
        file_package_name = entry->value;
    } else {
        
        for (i = 0; i < s->nb_streams; i++) {
            st = s->streams[i];
            if (entry = av_dict_get(st->metadata, "file_package_name", NULL, 0)) {
                file_package_name = entry->value;
                break;
            }
        }
    }
    mxf_write_preface(s);
    mxf_write_identification(s);
    mxf_write_content_storage(s);
    mxf_write_package(s, MaterialPackage, material_package_name);
    mxf_write_package(s, SourcePackage, file_package_name);
    mxf_write_essence_container_data(s);
    return 0;
}
