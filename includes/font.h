#ifndef __FONT__H__
#define __FONT__H__

typedef struct font {
    int w;
    int h;
    char * data;
} font_t;

font_t DEFAULT_FONT = {
    .w = 8,
    .h = 8,
    .data =
        // Char 32 (0x20) ' '
        "\x00\x00\x00\x00\x00\x00\x00\x00"
        // Char 33 (0x21) '!'
        "\x18\x18\x18\x18\x18\x00\x18\x00"
        // Char 34 (0x22) '"'
        "\x6C\x6C\x00\x00\x00\x00\x00\x00"
        // Char 35 (0x23) '#'
        "\x14\x14\x7E\x28\xFC\x50\x50\x00"
        // Char 36 (0x24) '$'
        "\x10\x7C\x90\x7C\x12\xFC\x10\x00"
        // Char 37 (0x25) '%'
        "\x62\x64\x08\x10\x26\x46\x00\x00"
        // Char 38 (0x26) '&'
        "\x38\x44\x44\x38\x4A\x44\x3A\x00"
        // Char 39 (0x27) '\''
        "\x30\x30\x00\x00\x00\x00\x00\x00"
        // Char 40 (0x28) '('
        "\x0C\x18\x30\x30\x30\x18\x0C\x00"
        // Char 41 (0x29) ')'
        "\x30\x18\x0C\x0C\x0C\x18\x30\x00"
        // Char 42 (0x2A) '*'
        "\x10\x54\x38\xFE\x38\x54\x10\x00"
        // Char 43 (0x2B) '+'
        "\x00\x18\x18\x7E\x7E\x18\x18\x00"
        // Char 44 (0x2C) ','
        "\x00\x00\x00\x00\x00\x18\x18\x30"
        // Char 45 (0x2D) '-'
        "\x00\x00\x00\x7E\x7E\x00\x00\x00"
        // Char 46 (0x2E) '.'
        "\x00\x00\x00\x00\x00\x18\x18\x00"
        // Char 47 (0x2F) '/'
        "\x02\x04\x08\x10\x20\x40\x80\x00"

        // Char 48 (0x30) '0' (slashed zero)
        "\x3C\x42\x46\x4A\x52\x62\x3C\x00"
        // Char 49 (0x31) '1'
        "\x08\x18\x38\x08\x08\x08\x3E\x00"
        // Char 50 (0x32) '2'
        "\x3C\x42\x02\x0C\x30\x40\x7E\x00"
        // Char 51 (0x33) '3'
        "\x3C\x42\x02\x1C\x02\x42\x3C\x00"
        // Char 52 (0x34) '4'
        "\x04\x0C\x14\x24\x7E\x04\x04\x00"
        // Char 53 (0x35) '5'
        "\x7E\x40\x7C\x02\x02\x42\x3C\x00"
        // Char 54 (0x36) '6'
        "\x1C\x20\x40\x7C\x42\x42\x3C\x00"
        // Char 55 (0x37) '7'
        "\x7E\x02\x04\x08\x10\x10\x10\x00"
        // Char 56 (0x38) '8'
        "\x3C\x42\x42\x3C\x42\x42\x3C\x00"
        // Char 57 (0x39) '9'
        "\x3C\x42\x42\x3E\x02\x04\x38\x00"
        // Char 58 (0x3A) ':'
        "\x00\x18\x18\x00\x00\x18\x18\x00"
        // Char 59 (0x3B) ';'
        "\x00\x18\x18\x00\x00\x18\x18\x30"
        // Char 60 (0x3C) '<'
        "\x04\x08\x10\x20\x10\x08\x04\x00"
        // Char 61 (0x3D) '='
        "\x00\x7E\x00\x00\x7E\x00\x00\x00"
        // Char 62 (0x3E) '>'
        "\x20\x10\x08\x04\x08\x10\x20\x00"
        // Char 63 (0x3F) '?'
        "\x3C\x42\x04\x08\x10\x00\x10\x00"

        // Char 64 (0x40) '@'
        "\x3C\x42\x9A\xAA\xAA\x9C\x40\x3C"
        // Char 65 (0x41) 'A'
        "\x18\x24\x42\x42\x7E\x42\x42\x00"
        // Char 66 (0x42) 'B'
        "\x7C\x42\x42\x7C\x42\x42\x7C\x00"
        // Char 67 (0x43) 'C'
        "\x3C\x42\x40\x40\x40\x42\x3C\x00"
        // Char 68 (0x44) 'D'
        "\x78\x44\x42\x42\x42\x44\x78\x00"
        // Char 69 (0x45) 'E'
        "\x7E\x40\x40\x7C\x40\x40\x7E\x00"
        // Char 70 (0x46) 'F'
        "\x7E\x40\x40\x7C\x40\x40\x40\x00"
        // Char 71 (0x47) 'G'
        "\x3C\x42\x40\x4E\x42\x42\x3C\x00"
        // Char 72 (0x48) 'H'
        "\x42\x42\x42\x7E\x42\x42\x42\x00"
        // Char 73 (0x49) 'I'
        "\x3E\x08\x08\x08\x08\x08\x3E\x00"
        // Char 74 (0x4A) 'J'
        "\x02\x02\x02\x02\x42\x42\x3C\x00"
        // Char 75 (0x4B) 'K'
        "\x44\x48\x50\x60\x50\x48\x44\x00"
        // Char 76 (0x4C) 'L'
        "\x40\x40\x40\x40\x40\x40\x7E\x00"
        // Char 77 (0x4D) 'M'
        "\x82\xC6\xAA\x92\x82\x82\x82\x00"
        // Char 78 (0x4E) 'N'
        "\x42\x62\x52\x4A\x46\x42\x42\x00"
        // Char 79 (0x4F) 'O'
        "\x3C\x42\x42\x42\x42\x42\x3C\x00"

        // Char 80 (0x50) 'P'
        "\x7C\x42\x42\x7C\x40\x40\x40\x00"
        // Char 81 (0x51) 'Q'
        "\x3C\x42\x42\x42\x4A\x44\x3A\x00"
        // Char 82 (0x52) 'R'
        "\x7C\x42\x42\x7C\x48\x44\x42\x00"
        // Char 83 (0x53) 'S'
        "\x3C\x42\x40\x3C\x02\x42\x3C\x00"
        // Char 84 (0x54) 'T'
        "\xFE\x10\x10\x10\x10\x10\x10\x00"
        // Char 85 (0x55) 'U'
        "\x42\x42\x42\x42\x42\x42\x3C\x00"
        // Char 86 (0x56) 'V'
        "\x42\x42\x42\x42\x24\x24\x18\x00"
        // Char 87 (0x57) 'W'
        "\x82\x82\x82\x92\xAA\xC6\x82\x00"
        // Char 88 (0x58) 'X'
        "\x42\x24\x18\x18\x18\x24\x42\x00"
        // Char 89 (0x59) 'Y'
        "\x82\x44\x28\x10\x10\x10\x10\x00"
        // Char 90 (0x5A) 'Z'
        "\x7E\x04\x08\x10\x20\x40\x7E\x00"
        // Char 91 (0x5B) '['
        "\x3C\x30\x30\x30\x30\x30\x3C\x00"
        // Char 92 (0x5C) '\'
        "\x80\x40\x20\x10\x08\x04\x02\x00"
        // Char 93 (0x5D) ']'
        "\x3C\x0C\x0C\x0C\x0C\x0C\x3C\x00"
        // Char 94 (0x5E) '^'
        "\x10\x28\x44\x00\x00\x00\x00\x00"
        // Char 95 (0x5F) '_'
        "\x00\x00\x00\x00\x00\x00\xFE\x00"

        // Char 96 (0x60) '`'
        "\x30\x18\x00\x00\x00\x00\x00\x00"
        // Char 97 (0x61) 'a'
        "\x00\x00\x3C\x02\x3E\x42\x3E\x00"
        // Char 98 (0x62) 'b'
        "\x40\x40\x7C\x42\x42\x42\x7C\x00"
        // Char 99 (0x63) 'c'
        "\x00\x00\x3C\x42\x40\x42\x3C\x00"
        // Char 100 (0x64) 'd'
        "\x02\x02\x3E\x42\x42\x42\x3E\x00"
        // Char 101 (0x65) 'e'
        "\x00\x00\x3C\x42\x7E\x40\x3C\x00"
        // Char 102 (0x66) 'f'
        "\x1C\x22\x20\x78\x20\x20\x20\x00"
        // Char 103 (0x67) 'g'
        "\x00\x00\x3E\x42\x42\x3E\x02\x3C"
        // Char 104 (0x68) 'h'
        "\x40\x40\x7C\x42\x42\x42\x42\x00"
        // Char 105 (0x69) 'i'
        "\x10\x00\x10\x10\x10\x10\x10\x00"
        // Char 106 (0x6A) 'j'
        "\x08\x00\x08\x08\x08\x48\x30\x00"
        // Char 107 (0x6B) 'k'
        "\x40\x40\x44\x48\x70\x48\x44\x00"
        // Char 108 (0x6C) 'l'
        "\x30\x10\x10\x10\x10\x10\x38\x00"
        // Char 109 (0x6D) 'm'
        "\x00\x00\xEC\x92\x92\x92\x92\x00"
        // Char 110 (0x6E) 'n'
        "\x00\x00\x7C\x42\x42\x42\x42\x00"
        // Char 111 (0x6F) 'o'
        "\x00\x00\x3C\x42\x42\x42\x3C\x00"

        // Char 112 (0x70) 'p'
        "\x00\x00\x7C\x42\x42\x7C\x40\x40"
        // Char 113 (0x71) 'q'
        "\x00\x00\x3E\x42\x42\x3E\x02\x02"
        // Char 114 (0x72) 'r'
        "\x00\x00\x5C\x62\x40\x40\x40\x00"
        // Char 115 (0x73) 's'
        "\x00\x00\x3E\x40\x3C\x02\x7C\x00"
        // Char 116 (0x74) 't'
        "\x20\x20\x78\x20\x20\x22\x1C\x00"
        // Char 117 (0x75) 'u'
        "\x00\x00\x42\x42\x42\x42\x3E\x00"
        // Char 118 (0x76) 'v'
        "\x00\x00\x42\x42\x24\x24\x18\x00"
        // Char 119 (0x77) 'w'
        "\x00\x00\x82\x92\x92\x54\x28\x00"
        // Char 120 (0x78) 'x'
        "\x00\x00\x42\x24\x18\x24\x42\x00"
        // Char 121 (0x79) 'y'
        "\x00\x00\x42\x42\x42\x3E\x02\x3C"
        // Char 122 (0x7A) 'z'
        "\x00\x00\x7E\x04\x18\x20\x7E\x00"
        // Char 123 (0x7B) '{'
        "\x0E\x18\x18\x70\x18\x18\x0E\x00"
        // Char 124 (0x7C) '|'
        "\x18\x18\x18\x18\x18\x18\x18\x00"
        // Char 125 (0x7D) '}'
        "\x70\x18\x18\x0E\x18\x18\x70\x00"
        // Char 126 (0x7E) '~'
        "\x32\x4C\x00\x00\x00\x00\x00\x00"
};

// font_t DEFAULT_FONT = {
//     .w = 8,
//     .h = 8,
//     .data =
//         // Char 32 (0x20) ' '
//         "\x00\x00\x00\x00\x00\x00\x00\x00"
//         // Char 33 (0x21) '!'
//         "\x18\x18\x18\x18\x18\x00\x18\x00"
//         // Char 34 (0x22) '"'
//         "\x66\x66\x24\x00\x00\x00\x00\x00"
//         // Char 35 (0x23) '#'
//         "\x00\x2A\x2A\x7E\x2A\x7E\x2A\x00"  // .X.X.   \x2A
//                                            // .X.X.   \x2A
//                                            // XXXXX   \x7E
//                                            // .X.X.   \x2A
//                                            // XXXXX   \x7E
//                                            // .X.X.   \x2A
//         // Char 36 (0x24) '$'
//         "\x10\x38\x50\x30\x14\x38\x10\x00"
//         // Char 37 (0x25) '%'
//         "\x00\x62\x64\x08\x10\x26\x46\x00"
//         // Char 38 (0x26) '&'
//         "\x38\x4C\x54\x28\x54\x4A\x34\x00"
//         // Char 39 (0x27) '''
//         "\x30\x30\x18\x00\x00\x00\x00\x00"
//         // Char 40 (0x28) '('
//         "\x0C\x18\x30\x30\x30\x18\x0C\x00"
//         // Char 41 (0x29) ')'
//         "\x30\x18\x0C\x0C\x0C\x18\x30\x00"
//         // Char 42 (0x2A) '*'
//         "\x00\x00\x42\x2A\x7E\x2A\x42\x00"
//         // Char 43 (0x2B) '+'
//         "\x00\x00\x18\x18\x7E\x18\x18\x00"
//         // Char 44 (0x2C) ','
//         "\x00\x00\x00\x00\x00\x18\x18\x0C"
//         // Char 45 (0x2D) '-'
//         "\x00\x00\x00\x7E\x00\x00\x00\x00"
//         // Char 46 (0x2E) '.'
//         "\x00\x00\x00\x00\x00\x00\x18\x18"
//         // Char 47 (0x2F) '/'
//         "\x00\x04\x08\x10\x20\x40\x40\x00"

//         // Char 48 (0x30) '0' (slashed zero)
//         "\x3C\x66\x6C\x72\x36\x26\x3C\x00"  // .XXX.   \x3C
//                                            // X..X.   \x66
//                                            // X.XX.   \x6C
//                                            // XX.X.   \x72
//                                            // .XX.X   \x36
//                                            // .X..X   \x26
//                                            // .XXX.   \x3C
//         // Char 49 (0x31) '1'
//         "\x18\x38\x18\x18\x18\x18\x3C\x00"
//         // Char 50 (0x32) '2'
//         "\x3C\x66\x0C\x18\x30\x60\x7E\x00"
//         // Char 51 (0x33) '3'
//         "\x3C\x06\x0C\x1C\x06\x66\x3C\x00"
//         // Char 52 (0x34) '4'
//         "\x0C\x1C\x3C\x6C\x7E\x0C\x0C\x00"
//         // Char 53 (0x35) '5'
//         "\x7E\x60\x60\x7C\x06\x06\x7C\x00"
//         // Char 54 (0x36) '6'
//         "\x3C\x60\x60\x7C\x66\x66\x3C\x00"
//         // Char 55 (0x37) '7'
//         "\x7E\x06\x0C\x18\x30\x30\x30\x00"
//         // Char 56 (0x38) '8'
//         "\x3C\x66\x66\x3C\x66\x66\x3C\x00"
//         // Char 57 (0x39) '9'
//         "\x3C\x66\x66\x3E\x06\x06\x3C\x00"
//         // Char 58 (0x3A) ':'
//         "\x00\x00\x18\x18\x00\x18\x18\x00"
//         // Char 59 (0x3B) ';'
//         "\x00\x00\x18\x18\x00\x18\x18\x0C"
//         // Char 60 (0x3C) '<'
//         "\x08\x1C\x38\x70\x38\x1C\x08\x00"
//         // Char 61 (0x3D) '='
//         "\x00\x00\x7E\x00\x00\x7E\x00\x00"  // spaced out
//         // Char 62 (0x3E) '>'
//         "\x10\x38\x3C\x0E\x3C\x38\x10\x00"
//         // Char 63 (0x3F) '?'
//         "\x3C\x66\x0C\x18\x18\x00\x18\x00"

//         // Char 64 (0x40) '@'
//         "\x3C\x66\x7A\x7A\x70\x60\x3E\x00"
//         // Char 65 (0x41) 'A'
//         "\x18\x3C\x66\x66\x7E\x66\x66\x00"
//         // Char 66 (0x42) 'B'
//         "\x7C\x66\x66\x7C\x66\x66\x7C\x00"
//         // Char 67 (0x43) 'C'
//         "\x3C\x66\x60\x60\x60\x66\x3C\x00"
//         // Char 68 (0x44) 'D'
//         "\x78\x6C\x66\x66\x66\x6C\x78\x00"
//         // Char 69 (0x45) 'E'
//         "\x7E\x60\x60\x7C\x60\x60\x7E\x00"
//         // Char 70 (0x46) 'F'
//         "\x7E\x60\x60\x7C\x60\x60\x60\x00"
//         // Char 71 (0x47) 'G'
//         "\x3C\x66\x60\x6E\x66\x66\x3C\x00"
//         // Char 72 (0x48) 'H'
//         "\x66\x66\x66\x7E\x66\x66\x66\x00"
//         // Char 73 (0x49) 'I'
//         "\x3C\x18\x18\x18\x18\x18\x3C\x00"
//         // Char 74 (0x4A) 'J'
//         "\x0C\x0C\x0C\x0C\xCC\xCC\x78\x00"
//         // Char 75 (0x4B) 'K'
//         "\x66\x6C\x78\x70\x78\x6C\x66\x00"
//         // Char 76 (0x4C) 'L'
//         "\x60\x60\x60\x60\x60\x60\x7E\x00"
//         // Char 77 (0x4D) 'M'
//         "\xC6\xEE\xFE\xD6\xC6\xC6\xC6\x00"
//         // Char 78 (0x4E) 'N'
//         "\xC6\xE6\xF6\xDE\xCE\xC6\xC6\x00"
//         // Char 79 (0x4F) 'O'
//         "\x3C\x66\x66\x66\x66\x66\x3C\x00"

//         // Char 80 (0x50) 'P'
//         "\x7C\x66\x66\x7C\x60\x60\x60\x00"
//         // Char 81 (0x51) 'Q'
//         "\x3C\x66\x66\x66\x6E\x7C\x0E\x00"
//         // Char 82 (0x52) 'R'
//         "\x7C\x66\x66\x7C\x6C\x66\x66\x00"
//         // Char 83 (0x53) 'S'
//         "\x3C\x60\x38\x0E\x06\x66\x3C\x00"
//         // Char 84 (0x54) 'T'
//         "\x7E\x18\x18\x18\x18\x18\x18\x00"
//         // Char 85 (0x55) 'U'
//         "\x66\x66\x66\x66\x66\x66\x3E\x00"
//         // Char 86 (0x56) 'V'
//         "\x66\x66\x66\x66\x3C\x18\x18\x00"
//         // Char 87 (0x57) 'W'
//         "\xC6\xC6\xC6\xD6\xFE\x6C\x6C\x00"
//         // Char 88 (0x58) 'X'
//         "\x66\x6C\x38\x1C\x38\x6C\x66\x00"
//         // Char 89 (0x59) 'Y'
//         "\x66\x66\x3C\x18\x18\x18\x18\x00"
//         // Char 90 (0x5A) 'Z'
//         "\x7E\x0C\x18\x30\x60\x60\x7E\x00"
//         // Char 91 (0x5B) '['
//         "\x3C\x30\x30\x30\x30\x30\x3C\x00"
//         // Char 92 (0x5C) '\'
//         "\x40\x40\x20\x10\x08\x04\x04\x00"
//         // Char 93 (0x5D) ']'
//         "\x3C\x0C\x0C\x0C\x0C\x0C\x3C\x00"
//         // Char 94 (0x5E) '^'
//         "\x18\x3C\x66\x00\x00\x00\x00\x00"
//         // Char 95 (0x5F) '_'
//         "\x00\x00\x00\x00\x00\x00\x00\xFF"

//         // Char 96 (0x60) '`'
//         "\x60\x30\x18\x00\x00\x00\x00\x00"
//         // Char 97 (0x61) 'a'
//         "\x00\x00\x3C\x06\x3E\x66\x3E\x00"
//         // Char 98 (0x62) 'b'
//         "\x60\x60\x7C\x66\x66\x66\x7C\x00"
//         // Char 99 (0x63) 'c'
//         "\x00\x00\x3C\x60\x60\x60\x3C\x00"
//         // Char 100 (0x64) 'd'
//         "\x06\x06\x3E\x66\x66\x66\x3E\x00"
//         // Char 101 (0x65) 'e'
//         "\x00\x00\x3C\x66\x7E\x60\x3C\x00"
//         // Char 102 (0x66) 'f'
//         "\x1C\x36\x30\x7C\x30\x30\x30\x00"
//         // Char 103 (0x67) 'g'
//         "\x00\x00\x3E\x66\x66\x3E\x06\x3C"  // Modern g
//         // Char 104 (0x68) 'h'
//         "\x60\x60\x7C\x66\x66\x66\x66\x00"
//         // Char 105 (0x69) 'i'
//         "\x18\x00\x18\x18\x18\x18\x18\x00"  // Distinct from 'l'
//         // Char 106 (0x6A) 'j'
//         "\x0C\x00\x0C\x0C\x0C\xCC\xCC\x78"
//         // Char 107 (0x6B) 'k'
//         "\x60\x60\x6C\x78\x70\x6C\x66\x00"
//         // Char 108 (0x6C) 'l'
//         "\x18\x18\x18\x18\x18\x18\x18\x00"  // Just a vertical bar
//         // Char 109 (0x6D) 'm'
//         "\x00\x00\xCC\xFE\xD6\xD6\xC6\x00"
//         // Char 110 (0x6E) 'n'
//         "\x00\x00\x7C\x66\x66\x66\x66\x00"
//         // Char 111 (0x6F) 'o'
//         "\x00\x00\x3C\x66\x66\x66\x3C\x00"

//         // Char 112 (0x70) 'p'
//         "\x00\x00\x7C\x66\x66\x7C\x60\x60"
//         // Char 113 (0x71) 'q'
//         "\x00\x00\x3E\x66\x66\x3E\x06\x06"
//         // Char 114 (0x72) 'r'
//         "\x00\x00\x7C\x66\x60\x60\x60\x00"
//         // Char 115 (0x73) 's'
//         "\x00\x00\x3E\x60\x3C\x06\x7C\x00"
//         // Char 116 (0x74) 't'
//         "\x10\x10\x7C\x10\x10\x12\x0C\x00"
//         // Char 117 (0x75) 'u'
//         "\x00\x00\x66\x66\x66\x66\x3E\x00"
//         // Char 118 (0x76) 'v'
//         "\x00\x00\x66\x66\x66\x3C\x18\x00"
//         // Char 119 (0x77) 'w'
//         "\x00\x00\xC6\xC6\xD6\xFE\x6C\x00"
//         // Char 120 (0x78) 'x'
//         "\x00\x00\x66\x3C\x18\x3C\x66\x00"
//         // Char 121 (0x79) 'y'
//         "\x00\x00\x66\x66\x3E\x0C\x18\x00"
//         // Char 122 (0x7A) 'z'
//         "\x00\x00\x7E\x0C\x18\x30\x7E\x00"
//         // Char 123 (0x7B) '{'
//         "\x1C\x30\x30\x60\x30\x30\x1C\x00" // .XX.  \x1C
//                                            // ..X.  \x30
//                                            // ..X.  \x30
//                                            // .X..  \x60
//                                            // ..X.  \x30
//                                            // ..X.  \x30
//                                            // .XX.  \x1C
//         // Char 124 (0x7C) '|'
//         "\x18\x18\x18\x18\x18\x18\x18\x00"
//         // Char 125 (0x7D) '}'
//         "\x38\x0C\x0C\x06\x0C\x0C\x38\x00"
//         // Char 126 (0x7E) '~'
//         "\x00\x00\x00\x2A\x54\x00\x00\x00"  // Centered & clean
// };


#endif /* __FONT__H__ */