C++ Calculator
A full-stack scientific and programmer calculator built with a real C++ backend, a custom mathematical expression parser, a width-aware programmer engine, and a responsive browser interface.
The application demonstrates that C++ can be used not only for native applications and systems programming, but also as the backend of a modern web application.
Live Application
Production deployment
https://scientific-calculator-cpp.onrender.com
Project Status
Current release: V10 Portfolio Release
Backend API version: 9.1
The calculator is deployed, containerized, automatically tested through GitHub Actions, and publicly accessible through Render.
Features
Scientific Calculator
The scientific calculator uses a custom C++ expression parser rather than JavaScript's eval().
Supported capabilities include:
- Addition
- Subtraction
- Multiplication
- Division
- Parentheses
- Operator precedence
- Unary positive and negative numbers
- Exponents
- Factorials
- Implicit multiplication
- Scientific constants
- Scientific functions
- Degree and radian trigonometry
- Calculation history
- Calculator memory
- Previous answer recall
- Copy expression
- Copy result
- Dark and light themes
Scientific Functions
Function	Description
sin(x)	Sine
cos(x)	Cosine
tan(x)	Tangent
log(x)	Base-10 logarithm
ln(x)	Natural logarithm
sqrt(x)	Square root
abs(x)	Absolute value
x!	Factorial
x^y	Exponentiation


Mathematical Constants
Constant	Meaning
pi	π
e	Euler's number


Example Expressions
2 + 3 * 4
(2 + 3) * 4
2^8 + 5!
sqrt(144)
sin(30)
sin(pi/2)
log(100) + ln(e)
Programmer Calculator
The programmer calculator is implemented by the C++ backend using width-aware integer operations.
Number Bases
The calculator supports:
- Binary — BIN
- Octal — OCT
- Decimal — DEC
- Hexadecimal — HEX
Changing the active number base automatically converts the current value.
Example:
DEC 255
becomes:
HEX FF
OCT 377
BIN 11111111
Word Sizes
The programmer engine supports:
- 8-bit
- 16-bit
- 32-bit
- 64-bit
Operations are restricted to the selected word size.
This allows the calculator to demonstrate real low-level integer behavior.
Signed and Unsigned Integers
Programmer mode supports both Unsigned and Signed interpretations.
For example, the 8-bit pattern:
11111111
can represent:
Unsigned: 255
Signed:   -1
This uses standard two's-complement representation.
Another example:
10000000
in an 8-bit word represents:
Unsigned: 128
Signed:   -128
Bitwise Operations
Supported programmer operations include:
Operation	Description
AND	Bitwise AND
OR	Bitwise OR
XOR	Bitwise exclusive OR
NOT	Bitwise complement
<<	Left shift
>>	Right shift
ROL	Rotate left
ROR	Rotate right


Example:
12 AND 10
Binary representation:
12 = 1100
10 = 1010
Result:
1000
Decimal:
8
Bit Editor
Programmer mode contains an interactive visual bit editor.
For an 8-bit word:
7 6 5 4 3 2 1 0
each bit can be clicked individually.
For example:
00000000
Clicking bit 7 produces:
10000000
The browser sends the operation to the C++ backend as a TOGGLE operation.
Integer Rotation
The calculator supports circular bit rotation.
Example using an 8-bit unsigned integer:
129
Binary:
10000001
Rotate left by one:
ROL 1
Result:
00000011
Decimal:
3
Rotate the result right by one:
ROR 1
returns:
10000001
Decimal:
129
Arithmetic Right Shift
When Signed mode is enabled, right shifting performs arithmetic sign extension.
Example:
-8 >> 1
Result:
-4
The sign bit is preserved by the C++ programmer engine.
Overflow Behavior
Programmer calculations follow a fixed-width overflow policy:
wrap-to-selected-width
For example, in an 8-bit word:
128 << 1
produces:
0
because:
10000000 << 1
creates:
1 00000000
and only the lowest eight bits are retained.
Programmer History
Programmer operations are stored separately from scientific calculations.
History records include:
- expression
- result
- number base
- selected word size
- signed/unsigned mode
Example:
12 AND 10 = 8
12 OR 10 = 14
12 XOR 10 = 6
History entries can be selected to restore previous programmer states.
Persistent State
The frontend stores calculator preferences in browser storage.
Programmer state can persist across page refreshes, including:
- calculator mode
- number base
- word size
- signed/unsigned mode
- current programmer value
- programmer history
Example:
Programmer
8-bit
Unsigned
HEX
FF
Refreshing the browser restores that state.
Technology Stack
Backend
- C++
- Drogon
- JsonCpp
- CMake
Frontend
- HTML5
- CSS3
- JavaScript
- Responsive browser interface
Infrastructure
- Docker
- GitHub Actions
- Render
API
- REST
- JSON
- HTTP
Architecture
The application uses a client-server architecture:
Browser
   |
   | HTTP / JSON
   v
Drogon REST API
   |
   v
C++ Calculation Engines
   |
   +----------------------+
   |                      |
   v                      v
Scientific Parser    Programmer Engine
The browser is responsible for:
- user interface
- calculator buttons
- history rendering
- state persistence
- keyboard interaction
- API requests
The C++ backend is responsible for:
- mathematical parsing
- scientific calculations
- programmer operations
- integer validation
- signed interpretation
- bit-width enforcement
- rotation
- shifting
- bit manipulation
- API responses
More details are available in:
docs/ARCHITECTURE.md
REST API
The application exposes four main endpoints.
Method	Endpoint	Purpose
GET	/health	Service health information
GET	/api/info	Application and API metadata
POST	/api/evaluate	Scientific expression evaluation
POST	/api/programmer	Programmer calculations


Complete API documentation:
docs/API.md
API Example — Scientific Calculator
Request:
POST /api/evaluate
Content-Type: application/json
Body:
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
API Example — Programmer Calculator
Request:
POST /api/programmer
Content-Type: application/json
Body:
{
  "operation": "AND",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
Response includes representations such as:
{
  "bin": "00001000",
  "dec": "8",
  "hex": "08",
  "oct": "010",
  "signedDec": "8",
  "unsignedDec": "8",
  "width": 8,
  "signed": false
}
API Metadata
The application provides machine-readable metadata at:
/api/info
Example metadata includes:
{
  "name": "C++ Calculator",
  "version": "9.1",
  "language": "C++",
  "framework": "Drogon"
}
The endpoint also describes:
- scientific functions
- programmer operations
- word sizes
- bases
- REST routes
- runtime technologies
- supported application features
Health Check
The service exposes:
GET /health
A successful response contains:
{
  "status": "ok"
}
The endpoint is used by automated tests and can also be used by hosting infrastructure for health monitoring.
Docker
The complete application runs inside Docker.
Build the image:
docker build -t scientific-calculator-cpp .
Run it:
docker run \
  --rm \
  -p 8080:8080 \
  scientific-calculator-cpp
Then open:
http://localhost:8080
Health endpoint:
http://localhost:8080/health
API information:
http://localhost:8080/api/info
Continuous Integration
GitHub Actions automatically validates the application whenever changes are pushed to the main branch or submitted through a pull request.
The CI pipeline:
1. checks out the repository
2. builds the Docker image
3. starts the C++ service
4. waits for the health endpoint
5. tests scientific calculations
6. tests degree and radian trigonometry
7. tests signed and unsigned programmer values
8. tests number conversion
9. tests AND, OR, XOR and NOT
10. tests shifts
11. tests rotate-left and rotate-right
12. tests bit toggling
13. tests invalid input handling
14. tests /api/info
15. validates important frontend features
This helps prevent regressions before deployment.
Project Structure
scientific-calculator-cpp/
│
├── backend/
│   ├── CMakeLists.txt
│   └── main.cpp
│
├── frontend/
│   ├── index.html
│   ├── style.css
│   └── app.js
│
├── docs/
│   ├── API.md
│   └── ARCHITECTURE.md
│
├── .github/
│   └── workflows/
│       └── build.yml
│
├── Dockerfile
├── LICENSE
└── README.md
Why C++?
C++ is normally associated with:
- operating systems
- embedded software
- game engines
- databases
- browsers
- networking
- performance-critical applications
This project explores another use case:
C++ as the backend of a modern web application.

Drogon provides the HTTP server and routing layer while the calculator engines remain implemented in C++.
This allows browser-based JavaScript to focus on interface behavior while calculation logic remains server-side.
Engineering Concepts Demonstrated
This project demonstrates practical experience with:
- C++ application development
- parsing algorithms
- recursion
- operator precedence
- REST APIs
- JSON request and response handling
- HTTP
- frontend/backend integration
- bit manipulation
- binary arithmetic
- two's-complement integers
- fixed-width integer behavior
- bit shifting
- bit rotation
- validation
- error handling
- Docker
- CMake
- CI/CD
- GitHub Actions
- responsive frontend development
- cloud deployment
Version History
V1
Initial scientific calculator.
V1.2
Improved calculator interaction and responsive interface.
V2
Additional scientific functions and constants.
V3
Custom C++ expression engine.
V4
Degree and radian trigonometry.
V5
Scientific history and memory.
V6
User-interface and usability improvements.
V7
Programmer calculator with:
- BIN
- OCT
- DEC
- HEX
- AND
- OR
- XOR
- NOT
- shifts
V8
Advanced programmer mode with:
- 8/16/32/64-bit words
- signed/unsigned integers
- two's complement
- ROL
- ROR
- arithmetic shift
- interactive bit editor
V9
Portfolio-oriented frontend improvements:
- programmer history
- persistent programmer state
- copy controls
- programmer keyboard support
- architecture display
- API display
V9.1
Backend metadata release:
- /api/info
- updated health metadata
- API capability discovery
- expanded CI validation
V10
Portfolio documentation release:
- professional README
- complete API documentation
- architecture documentation
- project structure documentation
- deployment instructions
- engineering concepts
- version history
Future Ideas
Possible future releases may include:
- floating-point bit visualization
- IEEE-754 converter
- binary32 and binary64 inspection
- engineering notation
- complex numbers
- matrices
- statistics
- unit conversion
- graphing
- expression variables
- downloadable calculation history
- API rate limiting
- OpenAPI specification
- automated releases
- version tags
- portfolio integration
Author
Joshua Dela Cruz
C++ / IT / Cloud / Cybersecurity learning portfolio project.
License
See LICENSE for the repository's license terms.
