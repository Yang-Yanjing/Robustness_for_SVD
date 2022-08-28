av_cold int ff_init_hscaler_mmxext(int dstW, int xInc, uint8_t *filterCode,
                                       int16_t *filter, int32_t *filterPos,
                                       int numSplits)
{
    uint8_t *fragmentA;
    x86_reg imm8OfPShufW1A;
    x86_reg imm8OfPShufW2A;
    x86_reg fragmentLengthA;
    uint8_t *fragmentB;
    x86_reg imm8OfPShufW1B;
    x86_reg imm8OfPShufW2B;
    x86_reg fragmentLengthB;
    int fragmentPos;
    int xpos, i;
    
    




    
    __asm__ volatile (
        "jmp                         9f                 \n\t"
        
        "0:                                             \n\t"
        "movq    (%%"REG_d", %%"REG_a"), %%mm3          \n\t"
        "movd    (%%"REG_c", %%"REG_S"), %%mm0          \n\t"
        "movd   1(%%"REG_c", %%"REG_S"), %%mm1          \n\t"
        "punpcklbw                %%mm7, %%mm1          \n\t"
        "punpcklbw                %%mm7, %%mm0          \n\t"
        "pshufw                   $0xFF, %%mm1, %%mm1   \n\t"
        "1:                                             \n\t"
        "pshufw                   $0xFF, %%mm0, %%mm0   \n\t"
        "2:                                             \n\t"
        "psubw                    %%mm1, %%mm0          \n\t"
        "movl   8(%%"REG_b", %%"REG_a"), %%esi          \n\t"
        "pmullw                   %%mm3, %%mm0          \n\t"
        "psllw                       $7, %%mm1          \n\t"
        "paddw                    %%mm1, %%mm0          \n\t"
        "movq                     %%mm0, (%%"REG_D", %%"REG_a") \n\t"
        "add                         $8, %%"REG_a"      \n\t"
        
        "9:                                             \n\t"
        "lea       " LOCAL_MANGLE(0b) ", %0             \n\t"
        "lea       " LOCAL_MANGLE(1b) ", %1             \n\t"
        "lea       " LOCAL_MANGLE(2b) ", %2             \n\t"
        "dec                         %1                 \n\t"
        "dec                         %2                 \n\t"
        "sub                         %0, %1             \n\t"
        "sub                         %0, %2             \n\t"
        "lea       " LOCAL_MANGLE(9b) ", %3             \n\t"
        "sub                         %0, %3             \n\t"
        : "=r" (fragmentA), "=r" (imm8OfPShufW1A), "=r" (imm8OfPShufW2A),
          "=r" (fragmentLengthA)
        );
    __asm__ volatile (
        "jmp                         9f                 \n\t"
        
        "0:                                             \n\t"
        "movq    (%%"REG_d", %%"REG_a"), %%mm3          \n\t"
        "movd    (%%"REG_c", %%"REG_S"), %%mm0          \n\t"
        "punpcklbw                %%mm7, %%mm0          \n\t"
        "pshufw                   $0xFF, %%mm0, %%mm1   \n\t"
        "1:                                             \n\t"
        "pshufw                   $0xFF, %%mm0, %%mm0   \n\t"
        "2:                                             \n\t"
        "psubw                    %%mm1, %%mm0          \n\t"
        "movl   8(%%"REG_b", %%"REG_a"), %%esi          \n\t"
        "pmullw                   %%mm3, %%mm0          \n\t"
        "psllw                       $7, %%mm1          \n\t"
        "paddw                    %%mm1, %%mm0          \n\t"
        "movq                     %%mm0, (%%"REG_D", %%"REG_a") \n\t"
        "add                         $8, %%"REG_a"      \n\t"
        
        "9:                                             \n\t"
        "lea       " LOCAL_MANGLE(0b) ", %0             \n\t"
        "lea       " LOCAL_MANGLE(1b) ", %1             \n\t"
        "lea       " LOCAL_MANGLE(2b) ", %2             \n\t"
        "dec                         %1                 \n\t"
        "dec                         %2                 \n\t"
        "sub                         %0, %1             \n\t"
        "sub                         %0, %2             \n\t"
        "lea       " LOCAL_MANGLE(9b) ", %3             \n\t"
        "sub                         %0, %3             \n\t"
        : "=r" (fragmentB), "=r" (imm8OfPShufW1B), "=r" (imm8OfPShufW2B),
          "=r" (fragmentLengthB)
        );
    xpos        = 0; 
    fragmentPos = 0;
    for (i = 0; i < dstW / numSplits; i++) {
        int xx = xpos >> 16;
        if ((i & 3) == 0) {
            int a                  = 0;
            int b                  = ((xpos + xInc) >> 16) - xx;
            int c                  = ((xpos + xInc * 2) >> 16) - xx;
            int d                  = ((xpos + xInc * 3) >> 16) - xx;
            int inc                = (d + 1 < 4);
            uint8_t *fragment      = inc ? fragmentB : fragmentA;
            x86_reg imm8OfPShufW1  = inc ? imm8OfPShufW1B : imm8OfPShufW1A;
            x86_reg imm8OfPShufW2  = inc ? imm8OfPShufW2B : imm8OfPShufW2A;
            x86_reg fragmentLength = inc ? fragmentLengthB : fragmentLengthA;
            int maxShift           = 3 - (d + inc);
            int shift              = 0;
            if (filterCode) {
                filter[i]        = ((xpos              & 0xFFFF) ^ 0xFFFF) >> 9;
                filter[i + 1]    = (((xpos + xInc)     & 0xFFFF) ^ 0xFFFF) >> 9;
                filter[i + 2]    = (((xpos + xInc * 2) & 0xFFFF) ^ 0xFFFF) >> 9;
                filter[i + 3]    = (((xpos + xInc * 3) & 0xFFFF) ^ 0xFFFF) >> 9;
                filterPos[i / 2] = xx;
                memcpy(filterCode + fragmentPos, fragment, fragmentLength);
                filterCode[fragmentPos + imm8OfPShufW1] =  (a + inc)       |
                                                          ((b + inc) << 2) |
                                                          ((c + inc) << 4) |
                                                          ((d + inc) << 6);
                filterCode[fragmentPos + imm8OfPShufW2] =  a | (b << 2) |
                                                               (c << 4) |
                                                               (d << 6);
                if (i + 4 - inc >= dstW)
                    shift = maxShift;               
                else if ((filterPos[i / 2] & 3) <= maxShift)
                    shift = filterPos[i / 2] & 3;   
                if (shift && i >= shift) {
                    filterCode[fragmentPos + imm8OfPShufW1] += 0x55 * shift;
                    filterCode[fragmentPos + imm8OfPShufW2] += 0x55 * shift;
                    filterPos[i / 2]                        -= shift;
                }
            }
            fragmentPos += fragmentLength;
            if (filterCode)
                filterCode[fragmentPos] = RET;
        }
        xpos += xInc;
    }
    if (filterCode)
        filterPos[((i / 2) + 1) & (~1)] = xpos >> 16;  
    return fragmentPos + 1;
}
