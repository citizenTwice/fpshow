[![macOS](https://img.shields.io/github/actions/workflow/status/citizenTwice/fpshow/build.yml?branch=main&job=build-mac&label=macOS)](https://github.com/citizenTwice/fpshow/actions/workflows/build.yml)
[![Linux](https://img.shields.io/github/actions/workflow/status/citizenTwice/fpshow/build.yml?branch=main&job=build-linux&label=Linux)](https://github.com/citizenTwice/fpshow/actions/workflows/build.yml)
[![Windows](https://img.shields.io/github/actions/workflow/status/citizenTwice/fpshow/build.yml?branch=main&job=build-windows&label=Windows)](https://github.com/citizenTwice/fpshow/actions/workflows/build.yml)

# fpshow
Convert/inspect the IEEE 754 bit representation of a floating-point number.

### Usage
```
Usage: fpshow [--f32|--f64|--raw32|--raw64] <value>

Inspect the IEEE 754 bit representation of a floating-point value.

Modes:
  --f32    Parse <value> as a 32-bit float  (default)
  --f64    Parse <value> as a 64-bit double
  --raw32  Interpret <value> as a raw 32-bit hex/integer bit pattern
  --raw64  Interpret <value> as a raw 64-bit hex/integer bit pattern

Arguments:
  <value>  Numeric value to inspect. For --raw32/--raw64, hex (0x...)
           and decimal integers are accepted.

Output:
  Sign, exponent, and fraction fields with their numeric values.
  Decoded value in mathematical form (handles ±0, ±inf, NaN, subnormals).
  Plain decimal and hex-float representations.
  Raw bit pattern with [sign][exponent][fraction] layout.
  ARMv8-A+ 8-bit immediate encoding eligibility and operand byte.
           e.g.: fmov s0, 1.125

Examples:
  fpshow 1.0
  fpshow --f32 -2.5
  fpshow --f64 3.141592653589793
  fpshow --raw32 0x3f800000
  fpshow --raw64 0x400921fb54442d18
```


#### License
MIT License

#### Feedback
Bugs/Suggestions/Comments/Feedback to:
Email: feedback@THLG.NL