#endif
block_t *DirBlock (access_t *p_access)
{
    access_sys_t *p_sys = p_access->p_sys;
    directory_t *current = p_sys->current;
    if (p_access->info.b_eof)
        return NULL;
    if (p_sys->header)
    {   /* Startup: send the XSPF header */
        static const char header[] =
            "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<playlist version=\"1\" xmlns=\"http://xspf.org/ns/0/\" xmlns:vlc=\"http://www.videolan.org/vlc/playlist/ns/0/\">\n"
            " <trackList>\n";
        block_t *block = block_Alloc (sizeof (header) - 1);
        if (!block)
            goto fatal;
        memcpy (block->p_buffer, header, sizeof (header) - 1);
        p_sys->header = false;
        return block;
    }
    if (current->i >= current->filec)
    {   /* End of directory, go back to parent */
        closedir (current->handle);
        p_sys->current = current->parent;
        free (current->uri);
        free (current->filev);
#ifndef HAVE_OPENAT
        free (current->path);
#endif
        free (current);
        if (p_sys->current == NULL)
        {   /* End of XSPF playlist */
            char *footer;
            int len = asprintf (&footer, " </trackList>\n"
                " <extension application=\"http://www.videolan.org/"
                                             "vlc/playlist/0\">\n"
                "%s"
                " </extension>\n"
                "</playlist>\n", p_sys->xspf_ext ? p_sys->xspf_ext : "");
            if (unlikely(len == -1))
                goto fatal;
            block_t *block = block_heap_Alloc (footer, len);
            p_access->info.b_eof = true;
            return block;
        }
        else
        {
            /* This was the end of a "subnode" */
            /* Write the ID to the extension */
            char *old_xspf_ext = p_sys->xspf_ext;
            if (old_xspf_ext != NULL
             && asprintf (&p_sys->xspf_ext, "%s  </vlc:node>\n",
                          old_xspf_ext ? old_xspf_ext : "") == -1)
                p_sys->xspf_ext = NULL;
            free (old_xspf_ext);
        }
        return NULL;
    }
    char *entry = current->filev[current->i++];
    /* Handle recursion */
    if (p_sys->mode != MODE_COLLAPSE)
    {
        DIR *handle;
#ifdef HAVE_OPENAT
        int fd = vlc_openat (dirfd (current->handle), entry,
                             O_RDONLY | O_DIRECTORY);
        if (fd == -1)
        {
            if (errno == ENOTDIR)
                goto notdir;
            goto skip; /* File cannot be opened... forget it */
        }
        struct stat st;
        if (fstat (fd, &st)
         || p_sys->mode == MODE_NONE
         || has_inode_loop (current, st.st_dev, st.st_ino)
         || (handle = fdopendir (fd)) == NULL)
        {
            close (fd);
            goto skip;
        }
#else
        char *path;
        if (asprintf (&path, "%s/%s", current->path, entry) == -1)
            goto skip;
        if ((handle = vlc_opendir (path)) == NULL)
            goto notdir;
        if (p_sys->mode == MODE_NONE)
            goto skip;
#endif
        directory_t *sub = malloc (sizeof (*sub));
        if (unlikely(sub == NULL))
        {
            closedir (handle);
#ifndef HAVE_OPENAT
            free (path);
#endif
            goto skip;
        }
        sub->parent = current;
        sub->handle = handle;
        sub->filec = vlc_loaddir (handle, &sub->filev, visible, p_sys->compar);
        if (sub->filec < 0)
            sub->filev = NULL;
        sub->i = 0;
#ifdef HAVE_OPENAT
        sub->device = st.st_dev;
        sub->inode = st.st_ino;
#else
        sub->path = path;
#endif
        p_sys->current = sub;
        char *encoded = encode_URI_component (entry);
        if (encoded == NULL
         || (asprintf (&sub->uri, "%s/%s", current->uri, encoded) == -1))
             sub->uri = NULL;
        free (encoded);
        if (unlikely(sub->uri == NULL))
        {
            free (entry);
            goto fatal;
        }
        /* Add node to XSPF extension */
        char *old_xspf_ext = p_sys->xspf_ext;
        EnsureUTF8 (entry);
        char *title = convert_xml_special_chars (entry);
        if (old_xspf_ext != NULL
         && asprintf (&p_sys->xspf_ext, "%s  <vlc:node title=\"%s\">\n",
                      old_xspf_ext, title ? title : "?") == -1)
            p_sys->xspf_ext = NULL;
        free (old_xspf_ext);
        free (title);
        goto skip;
    }
notdir:
    /* Skip files with ignored extensions */
    if (p_sys->ignored_exts != NULL)
    {
        const char *ext = strrchr (entry, '.');
        if (ext != NULL)
        {
            size_t extlen = strlen (++ext);
            for (const char *type = p_sys->ignored_exts, *end;
                 type[0]; type = end + 1)
            {
                end = strchr (type, ',');
                if (end == NULL)
                    end = type + strlen (type);
                if (type + extlen == end
                 && !strncasecmp (ext, type, extlen))
                {
                    free (entry);
                    return NULL;
                }
                if (*end == '\0')
                    break;
            }
        }
    }
    char *encoded = encode_URI_component (entry);
    free (entry);
    if (encoded == NULL)
        goto fatal;
    int len = asprintf (&entry,
                        "  <track><location>%s/%s</location>\n" \
                        "   <extension application=\"http://www.videolan.org/vlc/playlist/0\">\n" \
                        "    <vlc:id>%d</vlc:id>\n" \
                        "   </extension>\n" \
                        "  </track>\n",
                        current->uri, encoded, p_sys->i_item_count++);
    free (encoded);
    if (len == -1)
        goto fatal;
    /* Write the ID to the extension */
    char *old_xspf_ext = p_sys->xspf_ext;
    if (old_xspf_ext != NULL
     && asprintf (&p_sys->xspf_ext, "%s   <vlc:item tid=\"%i\" />\n",
                  old_xspf_ext, p_sys->i_item_count - 1) == -1)
        p_sys->xspf_ext = NULL;
    free (old_xspf_ext);
    block_t *block = block_heap_Alloc (entry, len);
    if (unlikely(block == NULL))
        goto fatal;
    return block;
fatal:
    p_access->info.b_eof = true;
    return NULL;
skip:
    free (entry);
    return NULL;
}
