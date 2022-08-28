int main(int argc, char *argv[])
{
    FILE *infile  = NULL;
    FILE *outfile = NULL;
    unsigned char atom_bytes[ATOM_PREAMBLE_SIZE];
    uint32_t atom_type   = 0;
    uint64_t atom_size   = 0;
    uint64_t atom_offset = 0;
    int64_t last_offset;
    unsigned char *moov_atom = NULL;
    unsigned char *ftyp_atom = NULL;
    uint64_t moov_atom_size;
    uint64_t ftyp_atom_size = 0;
    uint64_t i, j;
    uint32_t offset_count;
    uint64_t current_offset;
    int64_t start_offset = 0;
    unsigned char *copy_buffer = NULL;
    int bytes_to_copy;
    if (argc != 3) {
        printf("Usage: qt-faststart <infile.mov> <outfile.mov>\n"
               "Note: alternatively you can use -movflags +faststart in ffmpeg\n");
        return 0;
    }
    if (!strcmp(argv[1], argv[2])) {
        fprintf(stderr, "input and output files need to be different\n");
        return 1;
    }
    infile = fopen(argv[1], "rb");
    if (!infile) {
        perror(argv[1]);
        goto error_out;
    }
    

    while (!feof(infile)) {
        if (fread(atom_bytes, ATOM_PREAMBLE_SIZE, 1, infile) != 1) {
            break;
        }
        atom_size = BE_32(&atom_bytes[0]);
        atom_type = BE_32(&atom_bytes[4]);
        
        if (atom_type == FTYP_ATOM) {
            ftyp_atom_size = atom_size;
            free(ftyp_atom);
            ftyp_atom = malloc(ftyp_atom_size);
            if (!ftyp_atom) {
                printf("could not allocate %"PRIu64" bytes for ftyp atom\n",
                       atom_size);
                goto error_out;
            }
            if (fseeko(infile, -ATOM_PREAMBLE_SIZE, SEEK_CUR) ||
                fread(ftyp_atom, atom_size, 1, infile) != 1 ||
                (start_offset = ftello(infile)) < 0) {
                perror(argv[1]);
                goto error_out;
            }
        } else {
            int ret;
            
            if (atom_size == 1) {
                if (fread(atom_bytes, ATOM_PREAMBLE_SIZE, 1, infile) != 1) {
                    break;
                }
                atom_size = BE_64(&atom_bytes[0]);
                ret = fseeko(infile, atom_size - ATOM_PREAMBLE_SIZE * 2, SEEK_CUR);
            } else {
                ret = fseeko(infile, atom_size - ATOM_PREAMBLE_SIZE, SEEK_CUR);
            }
            if (ret) {
                perror(argv[1]);
                goto error_out;
            }
        }
        printf("%c%c%c%c %10"PRIu64" %"PRIu64"\n",
               (atom_type >> 24) & 255,
               (atom_type >> 16) & 255,
               (atom_type >>  8) & 255,
               (atom_type >>  0) & 255,
               atom_offset,
               atom_size);
        if ((atom_type != FREE_ATOM) &&
            (atom_type != JUNK_ATOM) &&
            (atom_type != MDAT_ATOM) &&
            (atom_type != MOOV_ATOM) &&
            (atom_type != PNOT_ATOM) &&
            (atom_type != SKIP_ATOM) &&
            (atom_type != WIDE_ATOM) &&
            (atom_type != PICT_ATOM) &&
            (atom_type != UUID_ATOM) &&
            (atom_type != FTYP_ATOM)) {
            printf("encountered non-QT top-level atom (is this a QuickTime file?)\n");
            break;
        }
        atom_offset += atom_size;
        


        if (atom_size < 8)
            break;
    }
    if (atom_type != MOOV_ATOM) {
        printf("last atom in file was not a moov atom\n");
        free(ftyp_atom);
        fclose(infile);
        return 0;
    }
    

    if (fseeko(infile, -atom_size, SEEK_END)) {
        perror(argv[1]);
        goto error_out;
    }
    last_offset    = ftello(infile);
    if (last_offset < 0) {
        perror(argv[1]);
        goto error_out;
    }
    moov_atom_size = atom_size;
    moov_atom      = malloc(moov_atom_size);
    if (!moov_atom) {
        printf("could not allocate %"PRIu64" bytes for moov atom\n", atom_size);
        goto error_out;
    }
    if (fread(moov_atom, atom_size, 1, infile) != 1) {
        perror(argv[1]);
        goto error_out;
    }
    

    if (BE_32(&moov_atom[12]) == CMOV_ATOM) {
        printf("this utility does not support compressed moov atoms yet\n");
        goto error_out;
    }
    
    fclose(infile);
    infile = NULL;
    
    for (i = 4; i < moov_atom_size - 4; i++) {
        atom_type = BE_32(&moov_atom[i]);
        if (atom_type == STCO_ATOM) {
            printf(" patching stco atom...\n");
            atom_size = BE_32(&moov_atom[i - 4]);
            if (i + atom_size - 4 > moov_atom_size) {
                printf(" bad atom size\n");
                goto error_out;
            }
            offset_count = BE_32(&moov_atom[i + 8]);
            if (i + 12 + offset_count * UINT64_C(4) > moov_atom_size) {
                printf(" bad atom size/element count\n");
                goto error_out;
            }
            for (j = 0; j < offset_count; j++) {
                current_offset  = BE_32(&moov_atom[i + 12 + j * 4]);
                current_offset += moov_atom_size;
                moov_atom[i + 12 + j * 4 + 0] = (current_offset >> 24) & 0xFF;
                moov_atom[i + 12 + j * 4 + 1] = (current_offset >> 16) & 0xFF;
                moov_atom[i + 12 + j * 4 + 2] = (current_offset >>  8) & 0xFF;
                moov_atom[i + 12 + j * 4 + 3] = (current_offset >>  0) & 0xFF;
            }
            i += atom_size - 4;
        } else if (atom_type == CO64_ATOM) {
            printf(" patching co64 atom...\n");
            atom_size = BE_32(&moov_atom[i - 4]);
            if (i + atom_size - 4 > moov_atom_size) {
                printf(" bad atom size\n");
                goto error_out;
            }
            offset_count = BE_32(&moov_atom[i + 8]);
            if (i + 12 + offset_count * UINT64_C(8) > moov_atom_size) {
                printf(" bad atom size/element count\n");
                goto error_out;
            }
            for (j = 0; j < offset_count; j++) {
                current_offset  = BE_64(&moov_atom[i + 12 + j * 8]);
                current_offset += moov_atom_size;
                moov_atom[i + 12 + j * 8 + 0] = (current_offset >> 56) & 0xFF;
                moov_atom[i + 12 + j * 8 + 1] = (current_offset >> 48) & 0xFF;
                moov_atom[i + 12 + j * 8 + 2] = (current_offset >> 40) & 0xFF;
                moov_atom[i + 12 + j * 8 + 3] = (current_offset >> 32) & 0xFF;
                moov_atom[i + 12 + j * 8 + 4] = (current_offset >> 24) & 0xFF;
                moov_atom[i + 12 + j * 8 + 5] = (current_offset >> 16) & 0xFF;
                moov_atom[i + 12 + j * 8 + 6] = (current_offset >>  8) & 0xFF;
                moov_atom[i + 12 + j * 8 + 7] = (current_offset >>  0) & 0xFF;
            }
            i += atom_size - 4;
        }
    }
    
    infile = fopen(argv[1], "rb");
    if (!infile) {
        perror(argv[1]);
        goto error_out;
    }
    if (start_offset > 0) { 
        if (fseeko(infile, start_offset, SEEK_SET)) {
            perror(argv[1]);
            goto error_out;
        }
        last_offset -= start_offset;
    }
    outfile = fopen(argv[2], "wb");
    if (!outfile) {
        perror(argv[2]);
        goto error_out;
    }
    
    if (ftyp_atom_size > 0) {
        printf(" writing ftyp atom...\n");
        if (fwrite(ftyp_atom, ftyp_atom_size, 1, outfile) != 1) {
            perror(argv[2]);
            goto error_out;
        }
    }
    
    printf(" writing moov atom...\n");
    if (fwrite(moov_atom, moov_atom_size, 1, outfile) != 1) {
        perror(argv[2]);
        goto error_out;
    }
    
    bytes_to_copy = MIN(COPY_BUFFER_SIZE, last_offset);
    copy_buffer = malloc(bytes_to_copy);
    if (!copy_buffer) {
        printf("could not allocate %d bytes for copy_buffer\n", bytes_to_copy);
        goto error_out;
    }
    printf(" copying rest of file...\n");
    while (last_offset) {
        bytes_to_copy = MIN(bytes_to_copy, last_offset);
        if (fread(copy_buffer, bytes_to_copy, 1, infile) != 1) {
            perror(argv[1]);
            goto error_out;
        }
        if (fwrite(copy_buffer, bytes_to_copy, 1, outfile) != 1) {
            perror(argv[2]);
            goto error_out;
        }
        last_offset -= bytes_to_copy;
    }
    fclose(infile);
    fclose(outfile);
    free(moov_atom);
    free(ftyp_atom);
    free(copy_buffer);
    return 0;
error_out:
    if (infile)
        fclose(infile);
    if (outfile)
        fclose(outfile);
    free(moov_atom);
    free(ftyp_atom);
    free(copy_buffer);
    return 1;
}
