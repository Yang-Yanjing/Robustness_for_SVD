static int
combineSeparateSamples8bits (uint8 *in[], uint8 *out, uint32 cols,
                            uint32 rows, uint16 spp, uint16 bps, 
 	                    FILE *dumpfile, int format, int level)
  {
  int    ready_bits = 0;
  
  uint32 src_rowsize, dst_rowsize, src_offset; 
  uint32 bit_offset;
  uint32 row, col, src_byte = 0, src_bit = 0;
  uint8  maskbits = 0, matchbits = 0;
  uint8  buff1 = 0, buff2 = 0;
  tsample_t s;
  unsigned char *src = in[0];
  unsigned char *dst = out;
  char           action[32];
  if ((src == NULL) || (dst == NULL))
    {
    TIFFError("combineSeparateSamples8bits","Invalid input or output buffer");
    return (1);
    }
   
  src_rowsize = ((bps * cols) + 7) / 8;
  dst_rowsize = ((bps * cols * spp) + 7) / 8;
  maskbits =  (uint8)-1 >> ( 8 - bps);
  for (row = 0; row < rows; row++)
    {
    ready_bits = 0;
    buff1 = buff2 = 0;
    dst = out + (row * dst_rowsize);
    src_offset = row * src_rowsize;
    for (col = 0; col < cols; col++)
      {
      
      bit_offset = col * bps;
      src_byte = bit_offset / 8;
      src_bit  = bit_offset % 8;
      matchbits = maskbits << (8 - src_bit - bps); 
      
      for (s = 0; (s < spp) && (s < MAX_SAMPLES); s++)
        {
	src = in[s] + src_offset + src_byte;
        buff1 = ((*src) & matchbits) << (src_bit);
        
        if (ready_bits >= 8)
          {
          *dst++ = buff2;
          buff2 = buff1;
          ready_bits -= 8;
          strcpy (action, "Flush");
          }
        else
          {
          buff2 = (buff2 | (buff1 >> ready_bits));
          strcpy (action, "Update");
          }
        ready_bits += bps;
        if ((dumpfile != NULL) && (level == 3))
          {
          dump_info (dumpfile, format, "",
                   "Row %3d, Col %3d, Samples %d, Src byte offset %3d  bit offset %2d  Dst offset %3d",
		   row + 1, col + 1, s, src_byte, src_bit, dst - out);
          dump_byte (dumpfile, format, "Match bits", matchbits);
          dump_byte (dumpfile, format, "Src   bits", *src);
          dump_byte (dumpfile, format, "Buff1 bits", buff1);
          dump_byte (dumpfile, format, "Buff2 bits", buff2);
          dump_info (dumpfile, format, "","%s", action); 
	  }
        }
      }
    if (ready_bits > 0)
      {
      buff1 = (buff2 & ((unsigned int)255 << (8 - ready_bits)));
      *dst++ = buff1;
      if ((dumpfile != NULL) && (level == 3))
        {
        dump_info (dumpfile, format, "",
	         "Row %3d, Col %3d, Src byte offset %3d  bit offset %2d  Dst offset %3d",
	         row + 1, col + 1, src_byte, src_bit, dst - out);
                 dump_byte (dumpfile, format, "Final bits", buff1);
        }
      }
    if ((dumpfile != NULL) && (level >= 2))
      {
      dump_info (dumpfile, format, "combineSeparateSamples8bits","Output data");
      dump_buffer(dumpfile, format, 1, dst_rowsize, row, out + (row * dst_rowsize));
      }
    }
  return (0);
  } 