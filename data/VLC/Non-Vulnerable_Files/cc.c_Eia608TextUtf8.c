}
static void Eia608TextUtf8( char *psz_utf8, uint8_t c ) // Returns number of bytes used
{
#define E1(c,u) { c, { u, '\0' } }
#define E2(c,u1,u2) { c, { u1, u2, '\0' } }
#define E3(c,u1,u2,u3) { c, { u1, u2, u3, '\0' } }
    static const struct {
        uint8_t c;
        char utf8[3+1];
    } c2utf8[] = {
        // Regular line-21 character set, mostly ASCII except these exceptions
        E2( 0x2a, 0xc3,0xa1), // lowercase a, acute accent
        E2( 0x5c, 0xc3,0xa9), // lowercase e, acute accent
        E2( 0x5e, 0xc3,0xad), // lowercase i, acute accent
        E2( 0x5f, 0xc3,0xb3), // lowercase o, acute accent
        E2( 0x60, 0xc3,0xba), // lowercase u, acute accent
        E2( 0x7b, 0xc3,0xa7), // lowercase c with cedilla
        E2( 0x7c, 0xc3,0xb7), // division symbol
        E2( 0x7d, 0xc3,0x91), // uppercase N tilde
        E2( 0x7e, 0xc3,0xb1), // lowercase n tilde
        // THIS BLOCK INCLUDES THE 16 EXTENDED (TWO-BYTE) LINE 21 CHARACTERS
        // THAT COME FROM HI BYTE=0x11 AND LOW BETWEEN 0x30 AND 0x3F
        E2( 0x80, 0xc2,0xae), // Registered symbol (R)
        E2( 0x81, 0xc2,0xb0), // degree sign
        E2( 0x82, 0xc2,0xbd), // 1/2 symbol
        E2( 0x83, 0xc2,0xbf), // Inverted (open) question mark
        E3( 0x84, 0xe2,0x84,0xa2), // Trademark symbol (TM)
        E2( 0x85, 0xc2,0xa2), // Cents symbol
        E2( 0x86, 0xc2,0xa3), // Pounds sterling
        E3( 0x87, 0xe2,0x99,0xaa), // Music note
        E2( 0x88, 0xc3,0xa0), // lowercase a, grave accent
        E2( 0x89, 0xc2,0xa0), // transparent space
        E2( 0x8a, 0xc3,0xa8), // lowercase e, grave accent
        E2( 0x8b, 0xc3,0xa2), // lowercase a, circumflex accent
        E2( 0x8c, 0xc3,0xaa), // lowercase e, circumflex accent
        E2( 0x8d, 0xc3,0xae), // lowercase i, circumflex accent
        E2( 0x8e, 0xc3,0xb4), // lowercase o, circumflex accent
        E2( 0x8f, 0xc3,0xbb), // lowercase u, circumflex accent
        // THIS BLOCK INCLUDES THE 32 EXTENDED (TWO-BYTE) LINE 21 CHARACTERS
        // THAT COME FROM HI BYTE=0x12 AND LOW BETWEEN 0x20 AND 0x3F
        E2( 0x90, 0xc3,0x81), // capital letter A with acute
        E2( 0x91, 0xc3,0x89), // capital letter E with acute
        E2( 0x92, 0xc3,0x93), // capital letter O with acute
        E2( 0x93, 0xc3,0x9a), // capital letter U with acute
        E2( 0x94, 0xc3,0x9c), // capital letter U with diaresis
        E2( 0x95, 0xc3,0xbc), // lowercase letter U with diaeresis
        E1( 0x96, 0x27), // apostrophe
        E2( 0x97, 0xc2,0xa1), // inverted exclamation mark
        E1( 0x98, 0x2a), // asterisk
        E1( 0x99, 0x27), // apostrophe (yes, duped). See CCADI source code.
        E1( 0x9a, 0x2d), // hyphen-minus
        E2( 0x9b, 0xc2,0xa9), // copyright sign
        E3( 0x9c, 0xe2,0x84,0xa0), // Service mark
        E1( 0x9d, 0x2e), // Full stop (.)
        E3( 0x9e, 0xe2,0x80,0x9c), // Quotation mark
        E3( 0x9f, 0xe2,0x80,0x9d), // Quotation mark
        E2( 0xa0, 0xc3,0x80), // uppercase A, grave accent
        E2( 0xa1, 0xc3,0x82), // uppercase A, circumflex
        E2( 0xa2, 0xc3,0x87), // uppercase C with cedilla
        E2( 0xa3, 0xc3,0x88), // uppercase E, grave accent
        E2( 0xa4, 0xc3,0x8a), // uppercase E, circumflex
        E2( 0xa5, 0xc3,0x8b), // capital letter E with diaresis
        E2( 0xa6, 0xc3,0xab), // lowercase letter e with diaresis
        E2( 0xa7, 0xc3,0x8e), // uppercase I, circumflex
        E2( 0xa8, 0xc3,0x8f), // uppercase I, with diaresis
        E2( 0xa9, 0xc3,0xaf), // lowercase i, with diaresis
        E2( 0xaa, 0xc3,0x94), // uppercase O, circumflex
        E2( 0xab, 0xc3,0x99), // uppercase U, grave accent
        E2( 0xac, 0xc3,0xb9), // lowercase u, grave accent
        E2( 0xad, 0xc3,0x9b), // uppercase U, circumflex
        E2( 0xae, 0xc2,0xab), // LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
        E2( 0xaf, 0xc2,0xbb), // RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
        // THIS BLOCK INCLUDES THE 32 EXTENDED (TWO-BYTE) LINE 21 CHARACTERS
        // THAT COME FROM HI BYTE=0x13 AND LOW BETWEEN 0x20 AND 0x3F
        E2( 0xb0, 0xc3,0x83), // Uppercase A, tilde
        E2( 0xb1, 0xc3,0xa3), // Lowercase a, tilde
        E2( 0xb2, 0xc3,0x8d), // Uppercase I, acute accent
        E2( 0xb3, 0xc3,0x8c), // Uppercase I, grave accent
        E2( 0xb4, 0xc3,0xac), // Lowercase i, grave accent
        E2( 0xb5, 0xc3,0x92), // Uppercase O, grave accent
        E2( 0xb6, 0xc3,0xb2), // Lowercase o, grave accent
        E2( 0xb7, 0xc3,0x95), // Uppercase O, tilde
        E2( 0xb8, 0xc3,0xb5), // Lowercase o, tilde
        E1( 0xb9, 0x7b), // Open curly brace
        E1( 0xba, 0x7d), // Closing curly brace
        E1( 0xbb, 0x5c), // Backslash
        E1( 0xbc, 0x5e), // Caret
        E1( 0xbd, 0x5f), // Underscore
        E2( 0xbe, 0xc2,0xa6), // Pipe (broken bar)
        E1( 0xbf, 0x7e), // Tilde (utf8 code unsure)
        E2( 0xc0, 0xc3,0x84), // Uppercase A, umlaut
        E2( 0xc1, 0xc3,0xa4), // Lowercase A, umlaut
        E2( 0xc2, 0xc3,0x96), // Uppercase O, umlaut
        E2( 0xc3, 0xc3,0xb6), // Lowercase o, umlaut
        E2( 0xc4, 0xc3,0x9f), // Esszett (sharp S)
        E2( 0xc5, 0xc2,0xa5), // Yen symbol
        E2( 0xc6, 0xc2,0xa4), // Currency symbol
        E1( 0xc7, 0x7c), // Vertical bar
        E2( 0xc8, 0xc3,0x85), // Uppercase A, ring
        E2( 0xc9, 0xc3,0xa5), // Lowercase A, ring
        E2( 0xca, 0xc3,0x98), // Uppercase O, slash
        E2( 0xcb, 0xc3,0xb8), // Lowercase o, slash
        E3( 0xcc, 0xe2,0x8c,0x9c), // Upper left corner
        E3( 0xcd, 0xe2,0x8c,0x9d), // Upper right corner
        E3( 0xce, 0xe2,0x8c,0x9e), // Lower left corner
        E3( 0xcf, 0xe2,0x8c,0x9f), // Lower right corner
        E1(0,0)
    };
#undef E3
#undef E2
#undef E1
    for( size_t i = 0; i < ARRAY_SIZE(c2utf8) ; i++ )
        if( c2utf8[i].c == c ) {
            strcpy( psz_utf8, c2utf8[i].utf8 );
            return;
        }
    psz_utf8[0] = c < 0x80 ? c : '?';   /* Normal : Unsupported */
    psz_utf8[1] = '\0';
}
