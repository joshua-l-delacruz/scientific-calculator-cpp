C++ Calculator REST API
This document describes the REST API exposed by the C++ Calculator backend.
The backend is implemented using:
- C++
- Drogon
- JsonCpp
The current backend API version is:
9.1
Base URL
Production:
https://scientific-calculator-cpp.onrender.com
Local Docker deployment:
http://localhost:8080
Endpoints
The application currently exposes:
Method	Endpoint
GET	/health
GET	/api/info
POST	/api/evaluate
POST	/api/programmer


GET /health
Returns basic service health and capability information.
Request
GET /health
Example Response
{
  "status": "ok",
  "service": "scientific-calculator-cpp",
  "name": "C++ Calculator",
  "version": "9.1",
  "language": "C++",
  "framework": "Drogon",
  "scientificMode": true,
  "programmerMode": true,
  "programmerWidths": "8,16,32,64",
  "signedProgrammerMode": true,
  "rotateOperations": "ROL,ROR",
  "interactiveBits": true,
  "angleModes": "DEG,RAD",
  "programmerBases": "BIN,OCT,DEC,HEX",
  "apiInfo": "/api/info"
}
GET /api/info
Returns machine-readable information about the application's supported capabilities.
Request
GET /api/info
Example Response
{
  "angleModes": [
    "DEG",
    "RAD"
  ],
  "description": "Full-stack scientific and programmer calculator powered by C++ and Drogon.",
  "features": {
    "bitEditor": true,
    "persistentFrontendState": true,
    "programmer": true,
    "programmerHistory": true,
    "scientific": true,
    "signedIntegers": true,
    "twoComplement": true,
    "unsignedIntegers": true
  },
  "framework": "Drogon",
  "language": "C++",
  "name": "C++ Calculator",
  "programmerBases": [
    "BIN",
    "OCT",
    "DEC",
    "HEX"
  ],
  "programmerOperations": [
    "CONVERT",
    "AND",
    "OR",
    "XOR",
    "NOT",
    "SHL",
    "SHR",
    "ROL",
    "ROR",
    "TOGGLE"
  ],
  "runtime": {
    "buildSystem": "CMake",
    "ci": "GitHub Actions",
    "container": "Docker",
    "containerized": true,
    "overflowPolicy": "wrap-to-selected-width"
  },
  "scientificFunctions": [
    "sin",
    "cos",
    "tan",
    "log",
    "ln",
    "sqrt",
    "abs",
    "factorial",
    "power"
  ],
  "version": "9.1",
  "wordSizes": [
    8,
    16,
    32,
    64
  ]
}
POST /api/evaluate
Evaluates a scientific mathematical expression.
Request Body
{
  "expression": "sin(30)",
  "mode": "DEG"
}
Fields
expression
Type:
string
Required:
yes
Maximum supported length:
500 characters
mode
Type:
string
Accepted values:
DEG
RAD
Default:
DEG
Scientific Example
Request:
{
  "expression": "2^8+5!",
  "mode": "DEG"
}
Response:
{
  "expression": "2^8+5!",
  "mode": "DEG",
  "result": 376
}
Trigonometry Example
Request:
{
  "expression": "sin(30)",
  "mode": "DEG"
}
Response:
{
  "expression": "sin(30)",
  "mode": "DEG",
  "result": 0.5
}
Radian Example
Request:
{
  "expression": "sin(pi/2)",
  "mode": "RAD"
}
Response:
{
  "expression": "sin(pi/2)",
  "mode": "RAD",
  "result": 1
}
Supported Scientific Syntax
Operators:
+
-
*
/
^
!
Grouping:
(
)
Constants:
pi
e
Functions:
sin()
cos()
tan()
log()
ln()
sqrt()
abs()
Scientific Errors
Example invalid expression:
{
  "expression": "2+",
  "mode": "DEG"
}
The server returns:
HTTP 400
with a response similar to:
{
  "error": "Unexpected end of expression."
}
POST /api/programmer
Performs integer conversion and bitwise operations.
Programmer Request Format
{
  "operation": "AND",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Programmer Fields
operation
Required.
Supported values:
CONVERT
AND
OR
XOR
NOT
SHL
SHR
ROL
ROR
TOGGLE
left
Required.
The primary input value.
Example:
"left": "255"
Values are strings because 64-bit unsigned integer values can exceed JavaScript's safe integer precision.
right
Required for operations that use a second operand.
Examples:
AND
OR
XOR
SHL
SHR
ROL
ROR
TOGGLE
Example:
"right": "1"
base
Required.
Supported:
BIN
OCT
DEC
HEX
width
Supported:
8
16
32
64
Default:
64
signed
Boolean.
true
or:
false
Default:
false
Programmer Conversion
Request:
{
  "operation": "CONVERT",
  "left": "255",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Example response:
{
  "base": "DEC",
  "bin": "11111111",
  "dec": "255",
  "engineVersion": "9.1",
  "hex": "FF",
  "oct": "377",
  "operation": "CONVERT",
  "overflowPolicy": "wrap-to-selected-width",
  "raw": "255",
  "signed": false,
  "signedDec": "-1",
  "unsignedDec": "255",
  "width": 8
}
Bitwise AND
Request:
{
  "operation": "AND",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Binary:
00001100
00001010
--------
00001000
Result:
8
Bitwise OR
Request:
{
  "operation": "OR",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Result:
14
Bitwise XOR
Request:
{
  "operation": "XOR",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Result:
6
Bitwise NOT
Request:
{
  "operation": "NOT",
  "left": "0",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Result:
255
Binary:
11111111
Left Shift
Request:
{
  "operation": "SHL",
  "left": "5",
  "right": "2",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Result:
20
Unsigned Right Shift
Request:
{
  "operation": "SHR",
  "left": "20",
  "right": "2",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Result:
5
Signed Arithmetic Right Shift
Request:
{
  "operation": "SHR",
  "left": "-8",
  "right": "1",
  "base": "DEC",
  "width": 8,
  "signed": true
}
Result:
-4
Rotate Left
Request:
{
  "operation": "ROL",
  "left": "129",
  "right": "1",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Before:
10000001
After:
00000011
Result:
3
Rotate Right
Request:
{
  "operation": "ROR",
  "left": "3",
  "right": "1",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Before:
00000011
After:
10000001
Result:
129
Toggle Bit
Request:
{
  "operation": "TOGGLE",
  "left": "0",
  "right": "7",
  "base": "DEC",
  "width": 8,
  "signed": false
}
The right field represents the bit index.
Before:
00000000
After toggling bit 7:
10000000
Result:
128
Signed Range Validation
For an 8-bit signed integer, the accepted decimal range is:
-128 to 127
Therefore:
{
  "operation": "CONVERT",
  "left": "128",
  "base": "DEC",
  "width": 8,
  "signed": true
}
returns an HTTP 400 error.
Unsigned Range
For an N-bit unsigned word:
0 to 2^N - 1
Examples:
Width	Minimum	Maximum
8	0	255
16	0	65535
32	0	4294967295
64	0	18446744073709551615


Signed Range
For an N-bit two's-complement word:
-2^(N-1)
through:
2^(N-1) - 1
Examples:
Width	Minimum	Maximum
8	-128	127
16	-32768	32767
32	-2147483648	2147483647
64	-9223372036854775808	9223372036854775807


Overflow Policy
Programmer operations use:
wrap-to-selected-width
The result is masked to the active word size.
CORS
The backend currently sends:
Access-Control-Allow-Origin: *
and supports:
Content-Type
with:
GET
POST
OPTIONS
methods as appropriate.
Error Format
API errors return JSON:
{
  "error": "Description of the error."
}
Common causes include:
- malformed JSON
- unsupported base
- unsupported word size
- invalid numeric digit
- signed value outside selected range
- missing operands
- invalid bit index
- unsupported programmer operation
- invalid scientific expression
