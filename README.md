# C++ Calculator

[![Build and Test](https://github.com/joshua-l-delacruz/scientific-calculator-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/joshua-l-delacruz/scientific-calculator-cpp/actions/workflows/build.yml)
[![C++](https://img.shields.io/badge/C%2B%2B-Backend-00599C?logo=cplusplus)](https://isocpp.org/)
[![Drogon](https://img.shields.io/badge/Drogon-Web%20Framework-blue)](https://github.com/drogonframework/drogon)
[![Docker](https://img.shields.io/badge/Docker-Containerized-2496ED?logo=docker&logoColor=white)](https://www.docker.com/)
[![Live](https://img.shields.io/badge/Live-Render-46E3B7?logo=render&logoColor=white)](https://scientific-calculator-cpp.onrender.com)

A full-stack **scientific and programmer calculator** powered by a real **C++ backend**, a custom mathematical expression parser, a width-aware programmer engine, REST APIs, Docker, and automated CI testing.

## Live Application

https://scientific-calculator-cpp.onrender.com

## Current Status

- Portfolio release: **V10.2**
- Backend API: **V9.1**
- Language: **C++**
- Web framework: **Drogon**
- Build system: **CMake**
- Container: **Docker**
- CI: **GitHub Actions**
- Hosting: **Render**

## Table of Contents

- [Features](#features)
- [Scientific Calculator](#scientific-calculator)
- [Programmer Calculator](#programmer-calculator)
- [Technology Stack](#technology-stack)
- [Architecture](#architecture)
- [REST API](#rest-api)
- [Docker](#docker)
- [Continuous Integration](#continuous-integration)
- [Project Structure](#project-structure)
- [What I Learned](#what-i-learned)
- [Version History](#version-history)
- [Author](#author)

# Features

## Scientific Calculator

The scientific calculator uses a custom recursive C++ expression parser instead of JavaScript `eval()`.

Supported features include:

- Addition
- Subtraction
- Multiplication
- Division
- Parentheses
- Operator precedence
- Unary positive and negative values
- Exponentiation
- Factorials
- Implicit multiplication
- Mathematical constants
- Scientific functions
- DEG/RAD trigonometry
- Memory functions
- Previous-answer recall
- Calculation history
- Copy expression
- Copy result
- Dark/light themes

### Scientific Functions

| Function | Description |
|---|---|
| `sin(x)` | Sine |
| `cos(x)` | Cosine |
| `tan(x)` | Tangent |
| `log(x)` | Base-10 logarithm |
| `ln(x)` | Natural logarithm |
| `sqrt(x)` | Square root |
| `abs(x)` | Absolute value |
| `x!` | Factorial |
| `x^y` | Exponentiation |

### Constants

| Constant | Meaning |
|---|---|
| `pi` | π |
| `e` | Euler's number |

### Examples

    2 + 3 * 4

    (2 + 3) * 4

    2^8 + 5!

    sqrt(144)

    sin(30)

    sin(pi/2)

    log(100) + ln(e)

# Programmer Calculator

The programmer calculator performs integer conversions and bitwise operations through the C++ backend.

## Number Bases

Supported bases:

- BIN — Binary
- OCT — Octal
- DEC — Decimal
- HEX — Hexadecimal

Example:

    DEC: 255
    HEX: FF
    OCT: 377
    BIN: 11111111

## Word Sizes

Supported integer widths:

- 8-bit
- 16-bit
- 32-bit
- 64-bit

Results are constrained to the selected word width.

## Signed and Unsigned Integers

The programmer engine supports both unsigned and signed two's-complement interpretation.

Example 8-bit value:

    11111111

Interpretations:

    Unsigned: 255
    Signed:   -1

Another example:

    10000000

Interpretations:

    Unsigned: 128
    Signed:   -128

## Bitwise Operations

| Operation | Description |
|---|---|
| `AND` | Bitwise AND |
| `OR` | Bitwise OR |
| `XOR` | Bitwise XOR |
| `NOT` | Bitwise complement |
| `<<` | Left shift |
| `>>` | Right shift |
| `ROL` | Rotate left |
| `ROR` | Rotate right |
| `TOGGLE` | Toggle individual bit |

Example:

    12 AND 10 = 8

Binary:

    12 = 1100
    10 = 1010
         ----
          1000

## Interactive Bit Editor

Programmer mode contains a clickable bit editor.

For an 8-bit word:

    7 6 5 4 3 2 1 0

Starting with:

    00000000

Toggling bit 7 produces:

    10000000

The browser sends a `TOGGLE` request to the C++ programmer API.

## Rotate Operations

Example:

    129 = 10000001

Rotate left by one:

    ROL 1

Result:

    00000011 = 3

Rotate right again:

    ROR 1

Result:

    10000001 = 129

## Arithmetic Right Shift

Signed mode performs arithmetic sign extension.

Example:

    -8 >> 1 = -4

## Overflow Behavior

Programmer operations use:

    wrap-to-selected-width

For example, in 8-bit mode:

    128 << 1

Binary:

    10000000 << 1
    1 00000000

Only the lowest eight bits remain:

    00000000

Result:

    0

## Programmer History

Programmer history stores:

- operation
- operands
- result
- number base
- word size
- signed/unsigned mode

Example:

    12 AND 10 = 8
    12 OR 10  = 14
    12 XOR 10 = 6

## Persistent State

The frontend remembers programmer settings across refreshes, including:

- calculator mode
- base
- word size
- signed/unsigned mode
- current value
- history

# Technology Stack

## Backend

- C++
- Drogon
- JsonCpp
- CMake

## Frontend

- HTML5
- CSS3
- JavaScript

## Infrastructure

- Docker
- GitHub Actions
- Render

## Communication

- REST
- HTTP
- JSON

# Architecture

    Browser
       |
       | HTTP / JSON
       v
    Drogon REST API
       |
       v
    C++ Backend
       |
       +----------------------+
       |                      |
       v                      v
    Scientific Parser    Programmer Engine

The browser handles:

- interface rendering
- keyboard interaction
- browser storage
- history presentation
- API requests

The C++ backend handles:

- expression parsing
- scientific calculations
- numeric validation
- number-base conversion
- fixed-width integers
- signed interpretation
- bitwise operations
- shifts
- rotations
- bit toggling
- API responses

Detailed architecture documentation:

[`docs/ARCHITECTURE.md`](docs/ARCHITECTURE.md)

# REST API

| Method | Endpoint | Purpose |
|---|---|---|
| GET | `/health` | Health and capability information |
| GET | `/api/info` | Application metadata |
| POST | `/api/evaluate` | Scientific calculations |
| POST | `/api/programmer` | Programmer calculations |

Full API documentation:

[`docs/API.md`](docs/API.md)

## Scientific API Example

Request:

    POST /api/evaluate

JSON body:

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

## Programmer API Example

Request:

    POST /api/programmer

JSON body:

    {
      "operation": "AND",
      "left": "12",
      "right": "10",
      "base": "DEC",
      "width": 8,
      "signed": false
    }

The response includes:

- binary representation
- octal representation
- decimal representation
- hexadecimal representation
- signed decimal interpretation
- unsigned decimal interpretation
- selected word width
- signed mode
- overflow policy

## API Metadata

Machine-readable API information is available at:

https://scientific-calculator-cpp.onrender.com/api/info

It describes:

- application version
- C++ framework
- available routes
- supported bases
- supported word sizes
- scientific functions
- programmer operations
- runtime technologies

# Docker

Build:

    docker build -t scientific-calculator-cpp .

Run:

    docker run --rm -p 8080:8080 scientific-calculator-cpp

Open:

    http://localhost:8080

Health check:

    http://localhost:8080/health

API metadata:

    http://localhost:8080/api/info

# Continuous Integration

GitHub Actions automatically builds and tests the project.

The pipeline validates:

- Docker image build
- C++ server startup
- health endpoint
- API metadata
- scientific arithmetic
- DEG trigonometry
- RAD trigonometry
- scientific functions
- invalid scientific expressions
- number conversions
- signed integers
- unsigned integers
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
- frontend availability

# Project Structure

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

# Why C++?

C++ is commonly associated with systems programming, game engines, embedded applications, databases, networking, browsers, and other performance-oriented software.

This project explores another use case:

> **C++ as the backend of a modern web application.**

Drogon provides the web-server and HTTP-routing layer while the authoritative calculation logic remains in C++.

# What I Learned

Building this project gave me practical experience with:

- modern C++ application development
- recursive-descent parsing
- operator precedence
- mathematical expression evaluation
- REST API design
- HTTP request handling
- JSON serialization
- frontend/backend integration
- binary arithmetic
- bit manipulation
- two's-complement representation
- fixed-width integer behavior
- logical and arithmetic shifting
- bit rotation
- input validation
- error handling
- Docker containers
- CMake
- GitHub Actions
- CI/CD
- responsive web interfaces
- cloud deployment

It also reinforced an important software-design principle:

> The frontend should manage presentation and interaction, while the backend remains authoritative for calculation and validation.

# Version History

## V1

Initial scientific calculator.

## V1.2

Responsive interface improvements.

## V2

Additional scientific functions and constants.

## V3

Custom recursive C++ expression parser.

## V4

DEG/RAD trigonometry.

## V5

Scientific memory and history.

## V6

Production-oriented UI improvements.

## V7

Initial programmer mode with:

- BIN
- OCT
- DEC
- HEX
- AND
- OR
- XOR
- NOT
- shifts

## V8

Advanced programmer engine:

- 8/16/32/64-bit words
- signed and unsigned integers
- two's complement
- arithmetic right shift
- ROL
- ROR
- interactive bit editor

## V9

Programmer usability and portfolio interface:

- programmer history
- persistent state
- copy controls
- keyboard support
- architecture section
- API section

## V9.1

Backend metadata release:

- `/api/info`
- expanded `/health`
- API capability metadata
- expanded CI tests

## V10

Portfolio documentation:

- professional README
- API documentation
- architecture documentation

## V10.1

GitHub presentation polish:

- build status badge
- technology badges
- live deployment badge
- table of contents
- recruiter-oriented project explanation
- What I Learned section

## V10.2

Desktop interface and programmer-mode reliability release:

- wide 1440p desktop layout
- independent Scientific and Programmer columns
- responsive 64-bit binary display
- corrected section spacing and overlap
- word-size error recovery
- versioned frontend assets for reliable deployment updates

# Future Ideas

Potential future enhancements:

- IEEE-754 floating-point visualization
- binary32/binary64 inspector
- engineering notation
- complex-number calculations
- matrices
- statistics
- graphing
- unit conversions
- variables
- downloadable history
- OpenAPI specification
- rate limiting
- structured logging
- automated GitHub releases
- semantic version tags
- portfolio website integration

# Author

**Joshua Dela Cruz**

C++ / IT / Cloud / Cybersecurity learning portfolio project.

# License

See [`LICENSE`](LICENSE) for license information.
