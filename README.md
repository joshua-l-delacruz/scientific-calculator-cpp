# C++ Calculator

A full-stack **scientific and programmer calculator** built with a real **C++ backend**, a custom mathematical expression parser, a width-aware programmer engine, and a responsive browser interface.

The application demonstrates that C++ can be used not only for native applications and systems programming, but also as the backend of a modern web application.

## Live Application

**Production deployment**

https://scientific-calculator-cpp.onrender.com

## Project Status

**Current release:** V10 Portfolio Release

**Backend API version:** 9.1

The calculator is deployed, containerized, automatically tested through GitHub Actions, and publicly accessible through Render.

---

# Features

## Scientific Calculator

The scientific calculator uses a custom C++ expression parser rather than JavaScript's `eval()`.

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

### Scientific Functions

Supported functions include:

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

### Mathematical Constants

| Constant | Meaning |
|---|---|
| `pi` | π |
| `e` | Euler's number |

### Example Expressions

```text
2 + 3 * 4
