# C++ Scientific & Programmer Calculator

[![Build and test](https://github.com/joshua-l-delacruz/scientific-calculator-cpp/actions/workflows/build.yml/badge.svg)](https://github.com/joshua-l-delacruz/scientific-calculator-cpp/actions/workflows/build.yml)
[![Live application](https://img.shields.io/badge/live-calculator.joshuadelacruz.solutions-2563EB)](https://calculator.joshuadelacruz.solutions/)
[![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

A full-stack scientific and fixed-width programmer calculator with an authoritative C++17/Drogon backend, a custom expression parser, REST APIs, Docker packaging and automated verification.

**[Open the live calculator](https://calculator.joshuadelacruz.solutions/)** · **[API reference](docs/API.md)** · **[Architecture](docs/ARCHITECTURE.md)**

![Scientific and programmer calculator](https://raw.githubusercontent.com/joshua-l-delacruz/lab-docs/main/assets/images/cpp-calculator-dashboard.png)

> The browser handles presentation and interaction. Scientific parsing, input validation, base conversion and programmer operations run in C++; the frontend does not use JavaScript `eval()`.

## Recruiter quick view

| Area | Evidence |
| --- | --- |
| **Problem** | Deliver scientific and fixed-width programmer calculations through one responsive browser interface. |
| **Engineering depth** | Recursive expression parsing, two's-complement interpretation, 8/16/32/64-bit arithmetic, bitwise operations, rotations and defined overflow behavior. |
| **Architecture** | Browser UI → JSON REST API → C++17/Drogon scientific and programmer engines. |
| **Verification** | GitHub Actions builds the production Docker image and checks health, metadata, calculations, invalid input and frontend delivery. |
| **Operations** | Health and metadata endpoints, request IDs, uptime reporting, container deployment and a branded canonical URL. |

## Capabilities

### Scientific mode

- Operator precedence, parentheses, unary operators and implicit multiplication
- Exponentiation, factorial, square root, absolute value and logarithms
- `sin`, `cos` and `tan` in degree or radian mode
- `pi` and `e` constants
- Memory, previous-answer recall, history and copy controls

Examples: `2 + 3 * 4`, `(2 + 3) * 4`, `sqrt(144)`, `sin(30)`, `log(100) + ln(e)`.

### Programmer mode

- Binary, octal, decimal and hexadecimal input/output
- 8-, 16-, 32- and 64-bit word sizes
- Signed and unsigned two's-complement interpretation
- `AND`, `OR`, `XOR`, `NOT`, left/right shifts, `ROL`, `ROR` and individual bit toggling
- Arithmetic right shift in signed mode
- Wrap-to-selected-width overflow behavior
- Persistent mode, base, width, signedness, current value and history settings

For example, `11111111` is `255` unsigned and `-1` signed in 8-bit mode. An 8-bit `128 << 1` wraps to `0` because only the lowest eight bits remain.

## Architecture

```text
Browser UI
    |
    | HTTP / JSON
    v
Drogon REST API
    |
    +--> Scientific parser
    |
    +--> Fixed-width programmer engine
```

The browser owns rendering, keyboard interaction, history presentation and local preferences. The backend owns parsing, calculations, validation, integer interpretation and API responses. See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for the detailed design.

## API

| Method | Endpoint | Purpose |
| --- | --- | --- |
| `GET` | `/health` | Service health, uptime and capability summary |
| `GET` | `/api/info` | Machine-readable application metadata |
| `POST` | `/api/evaluate` | Scientific expression evaluation |
| `POST` | `/api/programmer` | Base conversion and programmer operations |

Scientific request:

```json
{
  "expression": "sin(30)",
  "mode": "DEG"
}
```

Programmer request:

```json
{
  "operation": "AND",
  "left": "12",
  "right": "10",
  "base": "DEC",
  "width": 8,
  "signed": false
}
```

See [docs/API.md](docs/API.md) for request and response details.

## Run locally

```bash
docker build -t scientific-calculator-cpp .
docker run --rm -p 8080:8080 scientific-calculator-cpp
```

Open `http://localhost:8080`. Health and metadata are available at `/health` and `/api/info`.

## Verification

The GitHub Actions workflow builds the same multi-stage Docker image used for deployment, starts the service and verifies:

- health, API metadata and frontend delivery;
- scientific arithmetic, DEG/RAD trigonometry and invalid expressions;
- conversions, signed/unsigned ranges and supported word sizes;
- bitwise operations, shifts, rotations and bit toggling; and
- response security behavior.

## Security and scope

The application does not require accounts and should not collect personal information. Inputs are untrusted and are validated by the C++ backend. Calculation defects can use the public issue form; exploitable weaknesses should be reported privately according to [SECURITY.md](SECURITY.md).

This is an independently built portfolio project, not a safety-critical, financial or cryptographic calculation service. Verify results independently before using them for consequential decisions.

## Repository map

```text
backend/        C++17/Drogon server and calculation engines
frontend/       Accessible browser interface and discovery files
docs/           API and architecture documentation
.github/        CI, dependency updates and issue templates
Dockerfile      Reproducible multi-stage build
CHANGELOG.md    Release history
```

## Roadmap

- OpenAPI specification and structured logging
- Rate limiting and additional resource guards
- IEEE-754 visualization and engineering notation
- Complex numbers, matrices, statistics and graphing
- Automated releases and semantic version tags

## Contributing

Focused bug reports, calculation edge cases, accessibility improvements and tests are welcome. Read [CONTRIBUTING.md](CONTRIBUTING.md) before opening an issue or pull request.

## License

Licensed under the [MIT License](LICENSE).
