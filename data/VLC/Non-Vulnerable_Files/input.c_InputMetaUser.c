 *****************************************************************************/
static void InputMetaUser( input_thread_t *p_input, vlc_meta_t *p_meta )
{
    static const struct { int i_meta; const char *psz_name; } p_list[] = {
        { vlc_meta_Title,       "meta-title" },
        { vlc_meta_Artist,      "meta-artist" },
        { vlc_meta_Genre,       "meta-genre" },
        { vlc_meta_Copyright,   "meta-copyright" },
        { vlc_meta_Description, "meta-description" },
        { vlc_meta_Date,        "meta-date" },
        { vlc_meta_URL,         "meta-url" },
        { 0, NULL }
    };
