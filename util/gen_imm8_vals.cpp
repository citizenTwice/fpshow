// Print out all possible FP values compatible with ARM-V8a+ 8bit immediate operand form
// Coded by LuigiG
//
// clang++ -std=gnu++20 gen_imm8_vals.cpp -o gen_imm8_vals 
// ./gen_imm8_vals | grep fmov > imm8test.asm
// as imm8test.asm

#include <cmath>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <bitset>
#include <sstream>
#include <cstdint>
#include <cassert>
#include <format>

static std::string fmt_plain(const double d) {
  std::ostringstream sout;
  sout.precision(8);
  sout << std::fixed << d;
  return sout.str();
}
  
int main() {
  for (auto sign=0; sign <= 1 ; sign++) {
    for (auto bias=1; bias >= 0; bias--) {
      for (auto binexp = 0; binexp <= 3; binexp++) {
        for (auto int_offset = 0; int_offset <= 1; int_offset++) {
          double intpart = 2.0L + (int_offset * 1.0L);
          for (auto decmask = 0U; decmask <= 7; decmask++) {
            double decimals = 0.0L; // significand
            if (decmask & 1) {
              decimals += (0.125);
              // std::cout << "   with " << 0.125 << "\n";
            }
            if (decmask & 2) {
              decimals += (0.25);
              // std::cout << "   with " << 0.25 << "\n";
            }
            if (decmask & 4) {
              decimals += (0.5);
              // std::cout << "   with " << 0.5 << "\n";
            }
            double val_shift{2.0L};
            if (bias) {
              val_shift = pow(2.0L, double(-4 + (binexp & 3)));
            } else {
              val_shift = pow(2.0L, binexp & 3);
            }
            std::bitset<8> operand_enc = (sign << 7) | (bias << 6) | (binexp << 4) | (int_offset << 3) | decmask;
            double num =  (intpart + decimals) * val_shift;
            if (sign) { num = -num; }
            assert(!std::isnan(num));
            std::cout << "; sign: " << sign << " | bias: " << bias << 
               " | exp: " << binexp << " | mantissa offset: " << int_offset << " | fraction mask: 0x" << std::hex << decmask << "\n";
            std::cout << "Value:    " << fmt_plain(num) << "\n";
            std::cout << "Encoding: " << operand_enc.to_string() << ", // " << fmt_plain(num) << "\n"; 
            std::cout << "            fmov s0, " << fmt_plain(num) << "\n";
          }
        }
      }
    }
  }
  return 0;
}
