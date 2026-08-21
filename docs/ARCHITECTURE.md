# C++ Calculator Architecture

This document describes the architecture of the C++ Calculator web application.

The project combines a browser frontend with a C++ HTTP backend.

---

# High-Level Architecture

```text
┌─────────────────────────────┐
│          Browser            │
│                             │
│  HTML                       │
│  CSS                        │
│  JavaScript                 │
└──────────────┬──────────────┘
               │
               │ HTTP / JSON
               ▼
┌─────────────────────────────┐
│        Drogon Server        │
│                             │
│         C++ Backend         │
└──────────────┬──────────────┘
               │
               ├─────────────────────────┐
               │                         │
               ▼                         ▼
┌──────────────────────┐   ┌────────────────────────┐
│ Scientific Engine    │   │ Programmer Engine      │
│                      │   │                        │
│ Expression Parser    │   │ Width-aware integers   │
│ Functions            │   │ Bitwise operations     │
│ Trigonometry         │   │ Two's complement       │
│ Factorial            │   │ Shifts                 │
│ Powers               │   │ Rotations              │
└──────────────────────┘   └────────────────────────┘

Frontend Layer
The frontend is contained inside:
frontend/
and consists of:
index.html
style.css
app.js
index.html
index.html defines the user interface.
Major interface areas include:
- application header
- Scientific/Programmer mode selector
- scientific calculator
- programmer calculator
- bit editor
- history
- architecture information
- API information
- project information
style.css
style.css controls:
- dark theme
- light theme
- calculator layout
- responsive behavior
- scientific keypad
- programmer keypad
- bit editor
- number-base controls
- word-size controls
- API cards
- architecture cards
- mobile layout
The interface does not depend on a frontend framework.
app.js
app.js acts as the browser application controller.
Its responsibilities include:
- calculator state
- API communication
- scientific history
- programmer history
- memory functions
- DEG/RAD state
- word-size state
- signed/unsigned state
- base switching
- persistent browser storage
- keyboard controls
- bit-editor rendering
- copy-to-clipboard controls
- error messages
The browser does not perform the authoritative scientific or programmer calculations.
Those operations are delegated to the C++ backend.
Backend Layer
The backend is contained inside:
backend/
Primary source file:
backend/main.cpp
Build configuration:
backend/CMakeLists.txt
The server uses the Drogon C++ web framework.
Drogon Responsibilities
Drogon provides:
- HTTP server
- request routing
- request objects
- response objects
- JSON integration
- static frontend file serving
- asynchronous request callbacks
Backend Routes
The server exposes:
/
/style.css
/app.js
/health
/api/info
/api/evaluate
/api/programmer
Scientific Expression Engine
Scientific expressions are processed by a custom recursive parser.
The implementation does not depend on JavaScript's:
eval()
The parser processes expressions according to mathematical precedence.
Scientific Parsing Hierarchy
Conceptually:
Expression
    |
    v
Addition / Subtraction
    |
    v
Multiplication / Division
    |
    v
Unary Operators
    |
    v
Exponentiation
    |
    v
Postfix Operators
    |
    v
Primary Values
Expression Layer
Handles:
+
-
Example:
2 + 5 - 1
Term Layer
Handles:
*
/
and implicit multiplication.
Example:
2 * 5
or:
2pi
Unary Layer
Handles:
+5
-5
Power Layer
Handles:
2^8
Postfix Layer
Handles factorial:
5!
Primary Layer
Handles:
- numeric literals
- parentheses
- constants
- functions
Examples:
123
(2+3)
pi
sin(30)
Scientific Function Evaluation
Supported backend functions include:
sin
cos
tan
log
ln
sqrt
abs
Factorial and power operations are also handled by the parser.
Angle Conversion
The backend supports:
DEG
RAD
When DEG is active, trigonometric arguments are converted to radians before calling the C++ standard library.
Conceptually:
degrees × π / 180
Programmer Engine
The programmer calculator operates using:
uint64_t
as its internal storage representation.
Selected word sizes are enforced using masks.
Supported Word Sizes
8
16
32
64
Bit Mask
For a selected width N:
mask = 2^N - 1
except 64-bit mode, where the full uint64_t range is used.
After operations, values are masked to the selected width.
Signed Interpretation
Signed integer mode uses two's-complement interpretation.
For example:
11111111
as 8-bit unsigned:
255
as 8-bit signed:
-1
The raw bits are identical.
Only the interpretation changes.
Programmer Bases
Input can be interpreted as:
BIN
OCT
DEC
HEX
The C++ engine normalizes input and converts it to an internal integer value.
Responses include all four representations.
Bitwise AND
left & right
Example:
1100
1010
----
1000
Bitwise OR
left | right
Bitwise XOR
left ^ right
Bitwise NOT
~value
followed by masking to the selected word width.
Shift Left
value << amount
followed by width masking.
Shift Right
Unsigned mode performs logical right shifting.
Signed mode performs arithmetic right shifting.
Arithmetic shifting preserves the sign bit.
Rotate Left
Rotate-left moves bits beyond the most-significant position back into the least-significant side.
Example:
10000001
ROL 1:
00000011
Rotate Right
Rotate-right performs the opposite circular movement.
Example:
00000011
ROR 1:
10000001
Bit Toggle
The bit editor sends:
TOGGLE
with the selected bit index.
Conceptually:
value XOR (1 << bitIndex)
This changes:
0 → 1
or:
1 → 0
for that specific bit.
JSON API Layer
The frontend communicates with C++ using JSON.
Example request:
{
  "operation": "AND",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
The backend validates the request before performing the operation.
Why Programmer Values Are Strings
Programmer numbers are sent as JSON strings:
"left": "18446744073709551615"
rather than JavaScript numeric values.
This is important because JavaScript's normal Number type cannot precisely represent every 64-bit integer.
Using strings prevents precision loss before the value reaches C++.
Browser Storage
The frontend uses browser storage for usability.
Examples of stored state include:
- angle mode
- scientific memory
- previous scientific result
- scientific history
- selected calculator mode
- programmer base
- programmer width
- signed mode
- programmer value
- programmer history
- theme
This allows the calculator to recover interface state after refresh.
Docker Architecture
The application is packaged as a Docker image.
Conceptually:
GitHub Repository
       |
       v
Docker Build
       |
       v
C++ Compilation
       |
       v
Runtime Image
       |
       v
Drogon Server
Docker provides a consistent environment for:
- development
- GitHub Actions
- deployment
CMake
CMake is used to configure and compile the C++ application.
It connects the calculator executable with required libraries such as Drogon.
GitHub Actions
Continuous integration runs whenever changes are pushed or proposed through a pull request.
The pipeline:
Git Push
   |
   v
GitHub Actions
   |
   v
Docker Build
   |
   v
Start Container
   |
   v
Health Check
   |
   v
API Tests
   |
   v
Frontend Validation
Automated Backend Tests
The workflow checks:
- service health
- API metadata
- scientific arithmetic
- DEG mode
- RAD mode
- scientific functions
- invalid expressions
- unsigned conversion
- signed conversion
- AND
- OR
- XOR
- NOT
- left shift
- logical right shift
- arithmetic right shift
- rotate left
- rotate right
- bit toggling
- invalid signed ranges
- invalid word sizes
Deployment
The application is hosted on Render.
Production traffic follows:
User Browser
     |
     v
Render HTTPS Endpoint
     |
     v
Docker Container
     |
     v
Drogon C++ Server
Security Boundaries
The frontend is not trusted to enforce mathematical correctness.
The C++ API validates input independently.
Examples include:
- JSON validation
- expression size validation
- word-size validation
- base validation
- signed-range validation
- bit-index validation
- divide-by-zero validation
- logarithm domain validation
- square-root domain validation
This is important because browser-side controls can always be bypassed by direct API requests.
Design Philosophy
The architecture intentionally separates:
Presentation
from:
Calculation
The browser manages the interface.
The C++ server owns the calculation rules.
This makes the project easier to test and demonstrates a real frontend/backend architecture rather than a calculator implemented entirely inside the browser.
Future Architecture Possibilities
Future releases could introduce:
- multiple C++ source files
- controllers
- calculator service classes
- unit test targets
- OpenAPI generation
- structured logging
- API versioning
- Redis caching
- PostgreSQL history storage
- authentication
- user accounts
- rate limiting
- observability
- metrics
- dedicated frontend deployment
- load balancing
A larger production architecture could eventually become:
Browser
   |
   v
CDN
   |
   v
API Gateway
   |
   v
C++ Drogon Service
   |
   +--------+
   |        |
   v        v
Redis    PostgreSQL
The current architecture intentionally remains compact enough to understand while still demonstrating real full-stack C++ development.
