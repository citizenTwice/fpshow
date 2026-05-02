/*

MIT License

Copyright (c) 2026 Luigi Galli

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <signal.h>
#include <errno.h>

typedef enum MODE {MODE_F32, MODE_F64, MODE_RAW32, MODE_RAW64} FPMODE;

#define IMM8_TAB_SIZE 256
// ARM64 7 bit fp data
static const char* imm8_str_tab[] = {"0.12500000","0.13281250","0.14062500","0.14843750","0.15625000","0.16406250","0.17187500","0.17968750","0.18750000","0.19531250","0.20312500","0.21093750","0.21875000","0.22656250","0.23437500","0.24218750","0.25000000","0.26562500","0.28125000","0.29687500","0.31250000","0.32812500","0.34375000","0.35937500","0.37500000","0.39062500",
  "0.40625000","0.42187500","0.43750000","0.45312500","0.46875000","0.48437500","0.50000000","0.53125000","0.56250000","0.59375000","0.62500000","0.65625000","0.68750000","0.71875000","0.75000000","0.78125000","0.81250000","0.84375000","0.87500000","0.90625000","0.93750000","0.96875000","1.00000000","1.06250000","1.12500000","1.18750000",
  "1.25000000","1.31250000","1.37500000","1.43750000","1.50000000","1.56250000","1.62500000","1.68750000","1.75000000","1.81250000","1.87500000","1.93750000","2.00000000","2.12500000","2.25000000","2.37500000","2.50000000","2.62500000","2.75000000","2.87500000","3.00000000","3.12500000","3.25000000","3.37500000","3.50000000","3.62500000",
  "3.75000000","3.87500000","4.00000000","4.25000000","4.50000000","4.75000000","5.00000000","5.25000000","5.50000000","5.75000000","6.00000000","6.25000000","6.50000000","6.75000000","7.00000000","7.25000000","7.50000000","7.75000000","8.00000000","8.50000000","9.00000000","9.50000000","10.00000000","10.50000000","11.00000000","11.50000000",
  "12.00000000","12.50000000","13.00000000","13.50000000","14.00000000","14.50000000","15.00000000","15.50000000","16.00000000","17.00000000","18.00000000","19.00000000","20.00000000","21.00000000","22.00000000","23.00000000","24.00000000","25.00000000","26.00000000","27.00000000","28.00000000","29.00000000","30.00000000","31.00000000",
  "-0.12500000","-0.13281250","-0.14062500","-0.14843750","-0.15625000","-0.16406250","-0.17187500","-0.17968750","-0.18750000","-0.19531250","-0.20312500","-0.21093750","-0.21875000","-0.22656250","-0.23437500","-0.24218750","-0.25000000","-0.26562500","-0.28125000","-0.29687500","-0.31250000","-0.32812500","-0.34375000","-0.35937500",
  "-0.37500000","-0.39062500","-0.40625000","-0.42187500","-0.43750000","-0.45312500","-0.46875000","-0.48437500","-0.50000000","-0.53125000","-0.56250000","-0.59375000","-0.62500000","-0.65625000","-0.68750000","-0.71875000","-0.75000000","-0.78125000","-0.81250000","-0.84375000","-0.87500000","-0.90625000","-0.93750000","-0.96875000",
  "-1.00000000","-1.06250000","-1.12500000","-1.18750000","-1.25000000","-1.31250000","-1.37500000","-1.43750000","-1.50000000","-1.56250000","-1.62500000","-1.68750000","-1.75000000","-1.81250000","-1.87500000","-1.93750000","-2.00000000","-2.12500000","-2.25000000","-2.37500000","-2.50000000","-2.62500000","-2.75000000","-2.87500000",
  "-3.00000000","-3.12500000","-3.25000000","-3.37500000","-3.50000000","-3.62500000","-3.75000000","-3.87500000","-4.00000000","-4.25000000","-4.50000000","-4.75000000","-5.00000000","-5.25000000","-5.50000000","-5.75000000","-6.00000000","-6.25000000","-6.50000000","-6.75000000","-7.00000000","-7.25000000","-7.50000000","-7.75000000",
  "-8.00000000","-8.50000000","-9.00000000","-9.50000000","-10.00000000","-10.50000000","-11.00000000","-11.50000000","-12.00000000","-12.50000000","-13.00000000","-13.50000000","-14.00000000","-14.50000000","-15.00000000","-15.50000000","-16.00000000","-17.00000000","-18.00000000","-19.00000000","-20.00000000","-21.00000000","-22.00000000",
  "-23.00000000","-24.00000000","-25.00000000","-26.00000000","-27.00000000","-28.00000000","-29.00000000","-30.00000000","-31.00000000"
};
static uint8_t imm8_byte_tab[] = {
  0b01000000, 0b01000001, 0b01000010, 0b01000011, 0b01000100, 0b01000101, 0b01000110, 0b01000111, 0b01001000, 0b01001001, 0b01001010, 0b01001011, 0b01001100, 0b01001101, 0b01001110, 0b01001111, 0b01010000, 0b01010001, 0b01010010, 0b01010011, 0b01010100, 0b01010101, 0b01010110, 0b01010111, 0b01011000, 0b01011001, 0b01011010, 0b01011011, 0b01011100, 0b01011101, 0b01011110, 0b01011111, 0b01100000, 0b01100001, 0b01100010, 0b01100011, 0b01100100, 0b01100101, 0b01100110, 0b01100111, 0b01101000, 0b01101001, 0b01101010, 
  0b01101011, 0b01101100, 0b01101101, 0b01101110, 0b01101111, 0b01110000, 0b01110001, 0b01110010, 0b01110011, 0b01110100, 0b01110101, 0b01110110, 0b01110111, 0b01111000, 0b01111001, 0b01111010, 0b01111011, 0b01111100, 0b01111101, 0b01111110, 0b01111111, 0b00000000, 0b00000001, 0b00000010, 0b00000011, 0b00000100, 0b00000101, 0b00000110, 0b00000111, 0b00001000, 0b00001001, 0b00001010, 0b00001011, 0b00001100, 0b00001101, 0b00001110, 0b00001111, 0b00010000, 0b00010001, 0b00010010, 0b00010011, 0b00010100, 0b00010101, 
  0b00010110, 0b00010111, 0b00011000, 0b00011001, 0b00011010, 0b00011011, 0b00011100, 0b00011101, 0b00011110, 0b00011111, 0b00100000, 0b00100001, 0b00100010, 0b00100011, 0b00100100, 0b00100101, 0b00100110, 0b00100111, 0b00101000, 0b00101001, 0b00101010, 0b00101011, 0b00101100, 0b00101101, 0b00101110, 0b00101111, 0b00110000, 0b00110001, 0b00110010, 0b00110011, 0b00110100, 0b00110101, 0b00110110, 0b00110111, 0b00111000, 0b00111001, 0b00111010, 0b00111011, 0b00111100, 0b00111101, 0b00111110, 0b00111111, 0b11000000, 0b11000001, 
  0b11000010, 0b11000011, 0b11000100, 0b11000101, 0b11000110, 0b11000111, 0b11001000, 0b11001001, 0b11001010, 0b11001011, 0b11001100, 0b11001101, 0b11001110, 0b11001111, 0b11010000, 0b11010001, 0b11010010, 0b11010011, 0b11010100, 0b11010101, 0b11010110, 0b11010111, 0b11011000, 0b11011001, 0b11011010, 0b11011011, 0b11011100, 0b11011101, 0b11011110, 0b11011111, 0b11100000, 0b11100001, 0b11100010, 0b11100011, 0b11100100, 0b11100101, 0b11100110, 0b11100111, 0b11101000, 0b11101001, 0b11101010, 0b11101011, 0b11101100, 
  0b11101101, 0b11101110, 0b11101111, 0b11110000, 0b11110001, 0b11110010, 0b11110011, 0b11110100, 0b11110101, 0b11110110, 0b11110111, 0b11111000, 0b11111001, 0b11111010, 0b11111011, 0b11111100, 0b11111101, 0b11111110, 0b11111111, 0b10000000, 0b10000001, 0b10000010, 0b10000011, 0b10000100, 0b10000101, 0b10000110, 0b10000111, 0b10001000, 0b10001001, 0b10001010, 0b10001011, 0b10001100, 0b10001101, 0b10001110, 0b10001111, 0b10010000, 0b10010001, 0b10010010, 0b10010011, 0b10010100, 0b10010101, 0b10010110, 0b10010111, 0b10011000, 
  0b10011001, 0b10011010, 0b10011011, 0b10011100, 0b10011101, 0b10011110, 0b10011111, 0b10100000, 0b10100001, 0b10100010, 0b10100011, 0b10100100, 0b10100101, 0b10100110, 0b10100111, 0b10101000, 0b10101001, 0b10101010, 0b10101011, 0b10101100, 0b10101101, 0b10101110, 0b10101111, 0b10110000, 0b10110001, 0b10110010, 0b10110011, 0b10110100, 0b10110101, 0b10110110, 0b10110111, 0b10111000, 0b10111001, 0b10111010, 0b10111011, 0b10111100, 0b10111101, 0b10111110, 0b10111111
};

static void printbinx(uint64_t x, int bits) {
  if (bits <= 0) { return; }
  for (int i = bits; i >= 0; --i) {
    putchar((x & (UINT64_C(1) << i)) ? '1' : '0');
  }
}

int main(int ac, char **av) {
  static_assert(sizeof(float)  == 4, "size mismatch");
  static_assert(sizeof(double) == 8, "size mismatch");
  static_assert(FLT_RADIX      == 2, "size mismatch");
  static_assert(FLT_MANT_DIG   == 24, "size mismatch");
  static_assert(DBL_MANT_DIG   == 53, "size mismatch");
  static_assert(sizeof(strtoull("0",0,0)) >= sizeof(uint64_t), "size mismatch");
  static_assert(sizeof(strtoul("0",0,0)) >= sizeof(uint32_t), "size mismatch");
  static_assert(sizeof(imm8_str_tab)/sizeof(char*) == IMM8_TAB_SIZE, "table size mismatch");
  static_assert(sizeof(imm8_byte_tab)/sizeof(uint8_t) == IMM8_TAB_SIZE, "table size mismatch");

  double f64 = 0;
  float f32 = 0;
  uint32_t raw32 = 0;
  uint64_t raw64 = 0;
  double imm8_lkp_val = 0;
    
  const char* input = NULL;
  FPMODE mode = MODE_F32;

  if (ac < 2) {
usage: 
  printf("Usage: fpshow [--f32|--f64|--raw32|--raw64] <value>\n"
        "\n"
        "Inspect the IEEE 754 bit representation of a floating-point value.\n"
        "\n"
        "Modes:\n"
        "  --f32    Parse <value> as a 32-bit float  (default)\n"
        "  --f64    Parse <value> as a 64-bit double\n"
        "  --raw32  Interpret <value> as a raw 32-bit hex/integer bit pattern\n"
        "  --raw64  Interpret <value> as a raw 64-bit hex/integer bit pattern\n"
        "\n"
        "Arguments:\n"
        "  <value>  Numeric value to inspect. For --raw32/--raw64, hex (0x...)\n"
        "           and decimal integers are accepted.\n"
        "\n"
        "Output:\n"
        "  Sign, exponent, and fraction fields with their numeric values.\n"
        "  Decoded value in mathematical form (handles ±0, ±inf, NaN, subnormals).\n"
        "  Plain decimal and hex-float representations.\n"
        "  Raw bit pattern with [sign][exponent][fraction] layout.\n"
        "  ARMv8-A+ 8-bit immediate encoding eligibility and operand byte.\n"
        "           e.g.: fmov s0, 1.125\n"
        "\n"
        "Examples:\n"
        "  fpshow 1.0\n"
        "  fpshow --f32 -2.5\n"
        "  fpshow --f64 3.141592653589793\n"
        "  fpshow --raw32 0x3f800000\n"
        "  fpshow --raw64 0x400921fb54442d18\n");
     return 1;
  }
  bool opt_given = false;
  for (int i = 1; i < ac; i++) {
    if (*(av[i]) == '-' && *(av[i]+1) == '-') {
      if (opt_given) {
        fprintf(stderr, "options conflict\n");
        return 1;
      }
      opt_given = true;
      if (strcmp(av[i], "--f32") == 0) {
        mode = MODE_F32;
      } else if (strcmp(av[i], "--f64") == 0) {
        mode = MODE_F64;
      } else if (strcmp(av[i], "--raw32") == 0) {
        mode = MODE_RAW32;
      } else if (strcmp(av[i], "--raw64") == 0) {
        mode = MODE_RAW64;
      } else {
        goto usage;
      }
    } else {
      input = av[i];
    }
  }
  if (!input) {
    goto usage;
  }
  errno = 0;
  if (mode == MODE_F32) {
    f32 = strtof(input, 0);
    memcpy(&raw32, &f32, sizeof(raw32));
  } else if (mode == MODE_F64) {
    f64 = strtod(input, 0);
    memcpy(&raw64, &f64, sizeof(raw64));
  } else if (mode == MODE_RAW32) {
    raw32 = strtoul(input, 0, 0);
    memcpy(&f32, &raw32, sizeof(f32));
  } else if (mode == MODE_RAW64) {
    raw64 = strtoull(input, 0, 0);
    memcpy(&f64, &raw64, sizeof(f64));
  } else {
    assert(0 && "WTF");
    raise(SIGABRT);
  }
  if (errno) {
    fprintf(stderr, "input <%s> malformed or out of range\n", input);
    return 1;
  }
  
  if (mode == MODE_F32 || mode == MODE_RAW32) {
    uint32_t exponent = (raw32 & 0b01111111100000000000000000000000UL) >> 23;
    uint32_t sign     = (raw32 & 0b10000000000000000000000000000000UL) >> 31;
    uint32_t fraction =  raw32 & 0b00000000011111111111111111111111UL;
    printf("sign          : %x\n", sign);
    printf("exponent      : 0x%08x | %u\n", exponent, exponent);
    printf("fraction      : 0x%08x | %u\n", fraction, fraction);
    printf("value         : ");
    if (sign && exponent == 0 && fraction == 0) {
      printf("-0.0\n");
    } else if (!sign && exponent == 0 && fraction == 0) { 
      printf("+0.0\n");     
    } else if (sign && exponent == 0b011111111 && fraction == 0) { 
      printf("-infinity\n");
    } else if (!sign && exponent == 0b011111111 && fraction == 0) { 
      printf("+infinity\n");
    } else if (exponent == 0b011111111 && fraction != 0) { 
      printf("nan\n");
    } else {
      bool subnorm = false;
      if (exponent == 0 && fraction != 0) {
        subnorm = true; // e.g. 1.0e-40f
      }
      if (!subnorm) {
        printf("-1^%u * 1.%u/2^23 * 2^(%u - 127)\n", sign, fraction, exponent);
        printf("                ");
        printf("%c%.17lf * 2^%d\n", (sign ? '-' : '+'), 1.0F+(double)fraction/8388608.0F, ((int32_t)exponent) - 127);     
      } else {
        printf("-1^%u * 0.%u/2^23 * 2^(-126)\n", sign, fraction);
        printf("                ");
        printf("%c%.17lf * 2^(-126)\n", (sign ? '-' : '+'), (double)fraction/8388608.0F);     
      }
    }
    printf("printf %%.*g   : %.*g\n", FLT_DECIMAL_DIG, f32);
    printf("hex.fp        : %.6a\n", f32);
    printf("raw 32bit     : 0x%08x\n", raw32);
    printf("              : [");
    printf("%d", sign); printf("][");
    printbinx(exponent, 7); printf("][");
    printbinx(fraction, 22); printf("]\n");
    imm8_lkp_val = f32;
  } else if (mode == MODE_F64 || mode == MODE_RAW64) {
    uint64_t exponent = (raw64 & 0b0111111111110000000000000000000000000000000000000000000000000000UL) >> 52;
    uint64_t sign     = (raw64 & 0b1000000000000000000000000000000000000000000000000000000000000000UL) >> 63;
    uint64_t fraction =  raw64 & 0b0000000000001111111111111111111111111111111111111111111111111111UL;
    printf("sign          : %llx\n", sign);
    printf("exponent      : 0x%08llx       | %llu\n", exponent, exponent);
    printf("fraction      : 0x%014llx | %llu\n", fraction, fraction);
    printf("value         : ");
    if (sign && exponent == 0 && fraction == 0) {
      printf("-0.0\n");
    } else if (!sign && exponent == 0 && fraction == 0) { 
      printf("+0.0\n");     
    } else if (sign && exponent == 0b11111111111 && fraction == 0) { 
      printf("-infinity\n");
    } else if (!sign && exponent == 0b11111111111 && fraction == 0) { 
      printf("+infinity\n");
    } else if (exponent == 0b11111111111 && fraction != 0) { 
      printf("nan\n");
    } else {
      bool subnorm = false;
      if (exponent == 0 && fraction != 0) {
        subnorm = true; // e.g. 1.0e-40f
      }
      if (!subnorm) {
        printf("-1^%llu * 1.%llu/2^52 * 2^(%llu - 1023)\n", sign, fraction, exponent);
        printf("                ");
        printf("%c%.17lf * 2^%d\n", (sign ? '-' : '+'), 1.0F+(double)fraction/4503599627370496.0F, ((int32_t)exponent) - 1023);     
      } else {
        printf("-1^%llu * 0.%llu/2^52 * 2^(-1022)\n", sign, fraction);
        printf("                ");
        printf("%c%.17lf * 2^(-1022)\n", (sign ? '-' : '+'), (double)fraction/4503599627370496.0F);     
      }
    }
    // printf("default \%g   : %g\n",f64);
    printf("printf %%.*g   : %.*g\n", DBL_DECIMAL_DIG, f64);
    printf("hex.fp        : %.6a\n", f64);
    printf("raw 64bit     : 0x%016llx\n", raw64);
    printf("              : [");
    printf("%lld", sign); printf("][");
    printbinx(exponent, 10); printf("][");
    printbinx(fraction, 51); printf("]\n");
    imm8_lkp_val = f64;
  } else {
    assert(0);
    raise(SIGABRT);
  }
  // check for ARM64 8-bit immediate fp elegibility
  {
    char buf[128];
    snprintf(buf, sizeof(buf), "%.*f", 8, imm8_lkp_val);
    uint32_t index = 0;
    bool found = false;
    // we need an exact match - fall back to txt comparison
    // lame but effective
    for (int i = 0; i < IMM8_TAB_SIZE; i++) {
      if (strcmp(imm8_str_tab[i], buf) == 0) {
        found = true;
        index = i;
      }
    }
    printf("ARMv8-A+ fp   : 8 bit immediate form: ");
    if (found) {
      printf("yes - operand encoding = ");
      printbinx(imm8_byte_tab[index], 7);
      printf("\n");
      printf("                fmov s0, %s ; legal\n", buf);
      printf("\n");
    } else {
      printf("no\n");     
    }
  }  
  return 0;
}

