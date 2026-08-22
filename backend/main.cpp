#include <drogon/drogon.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <utility>

using namespace drogon;


// ============================================================
// APPLICATION METADATA
// ============================================================

constexpr const char *APPLICATION_NAME =
    "C++ Calculator";

constexpr const char *APPLICATION_VERSION =
    "9.1";

constexpr const char *APPLICATION_LANGUAGE =
    "C++";

constexpr const char *APPLICATION_FRAMEWORK =
    "Drogon";


// ============================================================
// MATHEMATICAL CONSTANTS
// ============================================================

constexpr double PI =
    3.141592653589793238462643383279502884;

constexpr double EULER_E =
    2.718281828459045235360287471352662498;


// ============================================================
// ANGLE MODE
// ============================================================

enum class AngleMode
{
    DEG,
    RAD
};


std::string angleModeToString(
    AngleMode mode
)
{
    return mode == AngleMode::RAD
        ? "RAD"
        : "DEG";
}


AngleMode parseAngleMode(
    std::string mode
)
{
    std::transform(
        mode.begin(),
        mode.end(),
        mode.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::toupper(character)
            );
        }
    );


    if (mode == "DEG")
    {
        return AngleMode::DEG;
    }


    if (mode == "RAD")
    {
        return AngleMode::RAD;
    }


    throw std::runtime_error(
        "Angle mode must be DEG or RAD."
    );
}


// ============================================================
// JSON HELPERS
// ============================================================

HttpResponsePtr jsonResponse(
    const Json::Value &json,
    HttpStatusCode status = k200OK
)
{
    auto response =
        HttpResponse::newHttpJsonResponse(
            json
        );


    response->setStatusCode(
        status
    );


    response->addHeader(
        "Access-Control-Allow-Origin",
        "*"
    );


    response->addHeader(
        "Access-Control-Allow-Headers",
        "Content-Type"
    );


    response->addHeader(
        "Access-Control-Allow-Methods",
        "GET, POST, OPTIONS"
    );


    return response;
}


HttpResponsePtr errorResponse(
    const std::string &message,
    HttpStatusCode status = k400BadRequest
)
{
    Json::Value json;


    json["error"] =
        message;


    return jsonResponse(
        json,
        status
    );
}


// ============================================================
// SERVER PORT
// ============================================================

int getServerPort()
{
    const char *portEnvironment =
        std::getenv(
            "PORT"
        );


    if (portEnvironment == nullptr)
    {
        return 8080;
    }


    try
    {
        return std::stoi(
            portEnvironment
        );
    }

    catch (...)
    {
        std::cerr
            << "Invalid PORT environment variable. "
            << "Using port 8080."
            << std::endl;


        return 8080;
    }
}


// ============================================================
// FACTORIAL
// ============================================================

double factorial(
    double value
)
{
    if (value < 0.0)
    {
        throw std::runtime_error(
            "Factorial requires a non-negative integer."
        );
    }


    const double rounded =
        std::round(
            value
        );


    if (
        std::abs(
            value - rounded
        ) > 1e-12
    )
    {
        throw std::runtime_error(
            "Factorial requires a whole number."
        );
    }


    if (rounded > 170.0)
    {
        throw std::runtime_error(
            "Factorial is limited to 170 to avoid overflow."
        );
    }


    double result =
        1.0;


    for (
        int i = 2;
        i <= static_cast<int>(rounded);
        ++i
    )
    {
        result *=
            static_cast<double>(i);
    }


    return result;
}


// ============================================================
// SCIENTIFIC EXPRESSION PARSER
// ============================================================

class ExpressionParser
{
private:

    std::string expression;

    std::size_t position =
        0;

    AngleMode angleMode =
        AngleMode::DEG;


public:

    ExpressionParser(
        std::string input,
        AngleMode mode
    )
        :
        expression(
            std::move(input)
        ),
        angleMode(
            mode
        )
    {
    }


    double parse()
    {
        position =
            0;


        const double result =
            parseExpression();


        skipWhitespace();


        if (
            position !=
            expression.length()
        )
        {
            throw std::runtime_error(
                "Unexpected character near position " +
                std::to_string(
                    position + 1
                ) +
                "."
            );
        }


        if (!std::isfinite(result))
        {
            throw std::runtime_error(
                "The calculation produced an invalid or overflowing result."
            );
        }


        return result;
    }


private:

    void skipWhitespace()
    {
        while (
            position <
                expression.length() &&
            std::isspace(
                static_cast<unsigned char>(
                    expression[position]
                )
            )
        )
        {
            ++position;
        }
    }


    bool match(
        char character
    )
    {
        skipWhitespace();


        if (
            position <
                expression.length() &&
            expression[position] ==
                character
        )
        {
            ++position;

            return true;
        }


        return false;
    }


    bool startsImplicitFactor()
    {
        skipWhitespace();


        if (
            position >=
            expression.length()
        )
        {
            return false;
        }


        const char character =
            expression[position];


        return (
            character == '(' ||
            character == '.' ||
            std::isdigit(
                static_cast<unsigned char>(
                    character
                )
            ) ||
            std::isalpha(
                static_cast<unsigned char>(
                    character
                )
            )
        );
    }


    double parseExpression()
    {
        double value =
            parseTerm();


        while (true)
        {
            if (match('+'))
            {
                value +=
                    parseTerm();
            }

            else if (match('-'))
            {
                value -=
                    parseTerm();
            }

            else
            {
                break;
            }
        }


        return value;
    }


    double parseTerm()
    {
        double value =
            parseUnary();


        while (true)
        {
            if (match('*'))
            {
                value *=
                    parseUnary();
            }

            else if (match('/'))
            {
                const double divisor =
                    parseUnary();


                if (
                    std::abs(divisor) <
                    1e-15
                )
                {
                    throw std::runtime_error(
                        "Cannot divide by zero."
                    );
                }


                value /=
                    divisor;
            }

            else if (
                startsImplicitFactor()
            )
            {
                value *=
                    parseUnary();
            }

            else
            {
                break;
            }
        }


        return value;
    }


    double parseUnary()
    {
        if (match('+'))
        {
            return parseUnary();
        }


        if (match('-'))
        {
            return -parseUnary();
        }


        return parsePower();
    }


    double parsePower()
    {
        double base =
            parsePostfix();


        if (match('^'))
        {
            const double exponent =
                parseUnary();


            base =
                std::pow(
                    base,
                    exponent
                );


            if (!std::isfinite(base))
            {
                throw std::runtime_error(
                    "Invalid power operation."
                );
            }
        }


        return base;
    }


    double parsePostfix()
    {
        double value =
            parsePrimary();


        while (match('!'))
        {
            value =
                factorial(
                    value
                );
        }


        return value;
    }


    double parsePrimary()
    {
        skipWhitespace();


        if (
            position >=
            expression.length()
        )
        {
            throw std::runtime_error(
                "Unexpected end of expression."
            );
        }


        if (match('('))
        {
            const double value =
                parseExpression();


            if (!match(')'))
            {
                throw std::runtime_error(
                    "Missing closing parenthesis."
                );
            }


            return value;
        }


        if (
            std::isdigit(
                static_cast<unsigned char>(
                    expression[position]
                )
            ) ||
            expression[position] == '.'
        )
        {
            return parseNumber();
        }


        if (
            std::isalpha(
                static_cast<unsigned char>(
                    expression[position]
                )
            )
        )
        {
            const std::string identifier =
                parseIdentifier();


            if (identifier == "pi")
            {
                return PI;
            }


            if (identifier == "e")
            {
                return EULER_E;
            }


            if (!match('('))
            {
                throw std::runtime_error(
                    "Expected '(' after function '" +
                    identifier +
                    "'."
                );
            }


            const double argument =
                parseExpression();


            if (!match(')'))
            {
                throw std::runtime_error(
                    "Missing closing parenthesis after function '" +
                    identifier +
                    "'."
                );
            }


            return evaluateFunction(
                identifier,
                argument
            );
        }


        throw std::runtime_error(
            "Unexpected character near position " +
            std::to_string(
                position + 1
            ) +
            "."
        );
    }


    double parseNumber()
    {
        skipWhitespace();


        const std::size_t start =
            position;


        bool decimalFound =
            false;

        bool digitFound =
            false;


        while (
            position <
            expression.length()
        )
        {
            const char character =
                expression[position];


            if (
                std::isdigit(
                    static_cast<unsigned char>(
                        character
                    )
                )
            )
            {
                digitFound =
                    true;

                ++position;

                continue;
            }


            if (
                character == '.' &&
                !decimalFound
            )
            {
                decimalFound =
                    true;

                ++position;

                continue;
            }


            break;
        }


        if (!digitFound)
        {
            throw std::runtime_error(
                "Invalid number."
            );
        }


        try
        {
            return std::stod(
                expression.substr(
                    start,
                    position - start
                )
            );
        }

        catch (...)
        {
            throw std::runtime_error(
                "Invalid number."
            );
        }
    }


    std::string parseIdentifier()
    {
        skipWhitespace();


        const std::size_t start =
            position;


        while (
            position <
                expression.length() &&
            std::isalpha(
                static_cast<unsigned char>(
                    expression[position]
                )
            )
        )
        {
            ++position;
        }


        std::string identifier =
            expression.substr(
                start,
                position - start
            );


        std::transform(
            identifier.begin(),
            identifier.end(),
            identifier.begin(),
            [](unsigned char character)
            {
                return static_cast<char>(
                    std::tolower(character)
                );
            }
        );


        return identifier;
    }


    double toRadians(
        double value
    ) const
    {
        return angleMode ==
            AngleMode::RAD
                ? value
                : value *
                    PI /
                    180.0;
    }


    double evaluateFunction(
        const std::string &name,
        double value
    )
    {
        if (name == "sqrt")
        {
            if (value < 0.0)
            {
                throw std::runtime_error(
                    "Square root requires a non-negative number."
                );
            }


            return std::sqrt(
                value
            );
        }


        if (name == "sin")
        {
            return std::sin(
                toRadians(
                    value
                )
            );
        }


        if (name == "cos")
        {
            return std::cos(
                toRadians(
                    value
                )
            );
        }


        if (name == "tan")
        {
            const double radians =
                toRadians(
                    value
                );


            if (
                std::abs(
                    std::cos(radians)
                ) <
                1e-12
            )
            {
                throw std::runtime_error(
                    "Tangent is undefined for this angle."
                );
            }


            return std::tan(
                radians
            );
        }


        if (name == "log")
        {
            if (value <= 0.0)
            {
                throw std::runtime_error(
                    "Logarithm requires a positive number."
                );
            }


            return std::log10(
                value
            );
        }


        if (name == "ln")
        {
            if (value <= 0.0)
            {
                throw std::runtime_error(
                    "Natural logarithm requires a positive number."
                );
            }


            return std::log(
                value
            );
        }


        if (name == "abs")
        {
            return std::abs(
                value
            );
        }


        throw std::runtime_error(
            "Unknown function: " +
            name +
            "."
        );
    }
};


// ============================================================
// PROGRAMMER HELPERS
// ============================================================

int programmerBaseFromName(
    std::string baseName
)
{
    std::transform(
        baseName.begin(),
        baseName.end(),
        baseName.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::toupper(character)
            );
        }
    );


    if (baseName == "BIN")
    {
        return 2;
    }


    if (baseName == "OCT")
    {
        return 8;
    }


    if (baseName == "DEC")
    {
        return 10;
    }


    if (baseName == "HEX")
    {
        return 16;
    }


    throw std::runtime_error(
        "Programmer base must be BIN, OCT, DEC or HEX."
    );
}


unsigned int parseBitWidth(
    unsigned int width
)
{
    if (
        width != 8 &&
        width != 16 &&
        width != 32 &&
        width != 64
    )
    {
        throw std::runtime_error(
            "Bit width must be 8, 16, 32 or 64."
        );
    }


    return width;
}


uint64_t maskForWidth(
    unsigned int width
)
{
    if (width == 64)
    {
        return std::numeric_limits<uint64_t>::max();
    }


    return (
        uint64_t{1} <<
        width
    ) - 1;
}


uint64_t signBitForWidth(
    unsigned int width
)
{
    return uint64_t{1} <<
        (
            width -
            1
        );
}


std::string normalizeIntegerInput(
    std::string text
)
{
    text.erase(
        std::remove_if(
            text.begin(),
            text.end(),
            [](unsigned char character)
            {
                return std::isspace(
                    character
                );
            }
        ),
        text.end()
    );


    std::transform(
        text.begin(),
        text.end(),
        text.begin(),
        [](unsigned char character)
        {
            return static_cast<char>(
                std::toupper(character)
            );
        }
    );


    return text;
}


uint64_t parseProgrammerValue(
    std::string text,
    int base,
    unsigned int width,
    bool signedMode
)
{
    text =
        normalizeIntegerInput(
            text
        );


    if (text.empty())
    {
        throw std::runtime_error(
            "Programmer value cannot be empty."
        );
    }


    const uint64_t mask =
        maskForWidth(
            width
        );


    // ========================================================
    // SIGNED DECIMAL INPUT
    // ========================================================

    if (
        signedMode &&
        base == 10
    )
    {
        try
        {
            std::size_t consumed =
                0;


            const long long value =
                std::stoll(
                    text,
                    &consumed,
                    10
                );


            if (
                consumed !=
                text.length()
            )
            {
                throw std::runtime_error(
                    "Invalid signed decimal value."
                );
            }


            if (width < 64)
            {
                const long long minimum =
                    -(
                        static_cast<long long>(
                            uint64_t{1} <<
                            (
                                width -
                                1
                            )
                        )
                    );


                const long long maximum =
                    static_cast<long long>(
                        (
                            uint64_t{1} <<
                            (
                                width -
                                1
                            )
                        ) -
                        1
                    );


                if (
                    value < minimum ||
                    value > maximum
                )
                {
                    throw std::runtime_error(
                        "Signed decimal value is outside the selected bit width."
                    );
                }
            }


            return
                static_cast<uint64_t>(
                    value
                ) &
                mask;
        }

        catch (
            const std::invalid_argument &
        )
        {
            throw std::runtime_error(
                "Invalid signed decimal value."
            );
        }

        catch (
            const std::out_of_range &
        )
        {
            throw std::runtime_error(
                "Signed decimal value is outside the selected bit width."
            );
        }
    }


    if (
        !text.empty() &&
        text[0] == '-'
    )
    {
        throw std::runtime_error(
            "Negative values require Signed DEC mode."
        );
    }


    if (
        base == 16 &&
        text.rfind(
            "0X",
            0
        ) == 0
    )
    {
        text =
            text.substr(2);
    }


    if (
        base == 2 &&
        text.rfind(
            "0B",
            0
        ) == 0
    )
    {
        text =
            text.substr(2);
    }


    if (text.empty())
    {
        throw std::runtime_error(
            "Invalid programmer value."
        );
    }


    try
    {
        std::size_t consumed =
            0;


        const unsigned long long value =
            std::stoull(
                text,
                &consumed,
                base
            );


        if (
            consumed !=
            text.length()
        )
        {
            throw std::runtime_error(
                "Invalid digit for the selected number base."
            );
        }


        if (
            static_cast<uint64_t>(
                value
            ) >
            mask
        )
        {
            throw std::runtime_error(
                "Value exceeds the selected bit width."
            );
        }


        return static_cast<uint64_t>(
            value
        );
    }

    catch (
        const std::invalid_argument &
    )
    {
        throw std::runtime_error(
            "Invalid digit for the selected number base."
        );
    }

    catch (
        const std::out_of_range &
    )
    {
        throw std::runtime_error(
            "Value exceeds the selected bit width."
        );
    }
}


std::string unsignedToBase(
    uint64_t value,
    int base,
    unsigned int minimumDigits = 1
)
{
    const char digits[] =
        "0123456789ABCDEF";


    std::string result;


    do
    {
        result.push_back(
            digits[
                value %
                static_cast<uint64_t>(
                    base
                )
            ]
        );


        value /=
            static_cast<uint64_t>(
                base
            );
    }
    while (value > 0);


    while (
        result.length() <
        minimumDigits
    )
    {
        result.push_back(
            '0'
        );
    }


    std::reverse(
        result.begin(),
        result.end()
    );


    return result;
}


std::string signedDecimalString(
    uint64_t raw,
    unsigned int width
)
{
    const uint64_t mask =
        maskForWidth(
            width
        );


    raw &=
        mask;


    const uint64_t signBit =
        signBitForWidth(
            width
        );


    if (
        (
            raw &
            signBit
        ) == 0
    )
    {
        return std::to_string(
            raw
        );
    }


    uint64_t magnitude;


    if (width == 64)
    {
        magnitude =
            (
                ~raw
            ) +
            1;
    }

    else
    {
        const uint64_t modulus =
            uint64_t{1} <<
            width;


        magnitude =
            modulus -
            raw;
    }


    return "-" +
        std::to_string(
            magnitude
        );
}


uint64_t arithmeticShiftRight(
    uint64_t value,
    unsigned int shift,
    unsigned int width
)
{
    const uint64_t mask =
        maskForWidth(
            width
        );


    value &=
        mask;


    if (shift == 0)
    {
        return value;
    }


    const bool negative =
        (
            value &
            signBitForWidth(
                width
            )
        ) != 0;


    uint64_t result =
        value >>
        shift;


    if (negative)
    {
        if (width == 64)
        {
            const uint64_t fillMask =
                std::numeric_limits<uint64_t>::max()
                <<
                (
                    64 -
                    shift
                );


            result |=
                fillMask;
        }

        else
        {
            const uint64_t fillMask =
                (
                    mask ^
                    (
                        (
                            uint64_t{1} <<
                            (
                                width -
                                shift
                            )
                        ) -
                        1
                    )
                );


            result |=
                fillMask;
        }
    }


    return result &
        mask;
}


uint64_t rotateLeft(
    uint64_t value,
    unsigned int amount,
    unsigned int width
)
{
    const uint64_t mask =
        maskForWidth(
            width
        );


    value &=
        mask;


    amount %=
        width;


    if (amount == 0)
    {
        return value;
    }


    if (width == 64)
    {
        return
            (
                value <<
                amount
            ) |
            (
                value >>
                (
                    64 -
                    amount
                )
            );
    }


    return
        (
            (
                value <<
                amount
            ) |
            (
                value >>
                (
                    width -
                    amount
                )
            )
        ) &
        mask;
}


uint64_t rotateRight(
    uint64_t value,
    unsigned int amount,
    unsigned int width
)
{
    const uint64_t mask =
        maskForWidth(
            width
        );


    value &=
        mask;


    amount %=
        width;


    if (amount == 0)
    {
        return value;
    }


    if (width == 64)
    {
        return
            (
                value >>
                amount
            ) |
            (
                value <<
                (
                    64 -
                    amount
                )
            );
    }


    return
        (
            (
                value >>
                amount
            ) |
            (
                value <<
                (
                    width -
                    amount
                )
            )
        ) &
        mask;
}


Json::Value programmerResultJson(
    uint64_t result,
    unsigned int width,
    bool signedMode
)
{
    const uint64_t mask =
        maskForWidth(
            width
        );


    result &=
        mask;


    const unsigned int hexDigits =
        width /
        4;


    const unsigned int octDigits =
        (
            width +
            2
        ) /
        3;


    Json::Value json;


    json["raw"] =
        std::to_string(
            result
        );


    json["bin"] =
        unsignedToBase(
            result,
            2,
            width
        );


    json["oct"] =
        unsignedToBase(
            result,
            8,
            octDigits
        );


    json["hex"] =
        unsignedToBase(
            result,
            16,
            hexDigits
        );


    json["unsignedDec"] =
        std::to_string(
            result
        );


    json["signedDec"] =
        signedDecimalString(
            result,
            width
        );


    json["dec"] =
        signedMode
            ? json["signedDec"]
            : json["unsignedDec"];


    json["width"] =
        width;


    json["signed"] =
        signedMode;


    json["overflowPolicy"] =
        "wrap-to-selected-width";


    return json;
}


// ============================================================
// API INFORMATION
// ============================================================

Json::Value buildApiInfo()
{
    Json::Value json;


    json["name"] =
        APPLICATION_NAME;


    json["version"] =
        APPLICATION_VERSION;


    json["language"] =
        APPLICATION_LANGUAGE;


    json["framework"] =
        APPLICATION_FRAMEWORK;


    json["description"] =
        "Full-stack scientific and programmer calculator powered by C++ and Drogon.";


    // ========================================================
    // FEATURES
    // ========================================================

    Json::Value features;


    features["scientific"] =
        true;


    features["programmer"] =
        true;


    features["signedIntegers"] =
        true;


    features["unsignedIntegers"] =
        true;


    features["bitEditor"] =
        true;


    features["twoComplement"] =
        true;


    features["persistentFrontendState"] =
        true;


    features["programmerHistory"] =
        true;


    json["features"] =
        features;


    // ========================================================
    // ANGLE MODES
    // ========================================================

    Json::Value angleModes(
        Json::arrayValue
    );


    angleModes.append(
        "DEG"
    );


    angleModes.append(
        "RAD"
    );


    json["angleModes"] =
        angleModes;


    // ========================================================
    // PROGRAMMER BASES
    // ========================================================

    Json::Value bases(
        Json::arrayValue
    );


    bases.append(
        "BIN"
    );


    bases.append(
        "OCT"
    );


    bases.append(
        "DEC"
    );


    bases.append(
        "HEX"
    );


    json["programmerBases"] =
        bases;


    // ========================================================
    // WORD SIZES
    // ========================================================

    Json::Value wordSizes(
        Json::arrayValue
    );


    wordSizes.append(
        8
    );


    wordSizes.append(
        16
    );


    wordSizes.append(
        32
    );


    wordSizes.append(
        64
    );


    json["wordSizes"] =
        wordSizes;


    // ========================================================
    // PROGRAMMER OPERATIONS
    // ========================================================

    Json::Value operations(
        Json::arrayValue
    );


    operations.append(
        "CONVERT"
    );


    operations.append(
        "AND"
    );


    operations.append(
        "OR"
    );


    operations.append(
        "XOR"
    );


    operations.append(
        "NOT"
    );


    operations.append(
        "SHL"
    );


    operations.append(
        "SHR"
    );


    operations.append(
        "ROL"
    );


    operations.append(
        "ROR"
    );


    operations.append(
        "TOGGLE"
    );


    json["programmerOperations"] =
        operations;


    // ========================================================
    // SCIENTIFIC FUNCTIONS
    // ========================================================

    Json::Value scientificFunctions(
        Json::arrayValue
    );


    scientificFunctions.append(
        "sin"
    );


    scientificFunctions.append(
        "cos"
    );


    scientificFunctions.append(
        "tan"
    );


    scientificFunctions.append(
        "log"
    );


    scientificFunctions.append(
        "ln"
    );


    scientificFunctions.append(
        "sqrt"
    );


    scientificFunctions.append(
        "abs"
    );


    scientificFunctions.append(
        "factorial"
    );


    scientificFunctions.append(
        "power"
    );


    json["scientificFunctions"] =
        scientificFunctions;


    // ========================================================
    // API ROUTES
    // ========================================================

    Json::Value routes(
        Json::arrayValue
    );


    Json::Value healthRoute;

    healthRoute["method"] =
        "GET";

    healthRoute["path"] =
        "/health";

    healthRoute["description"] =
        "Service health and capability summary";


    routes.append(
        healthRoute
    );


    Json::Value infoRoute;

    infoRoute["method"] =
        "GET";

    infoRoute["path"] =
        "/api/info";

    infoRoute["description"] =
        "Application metadata and supported features";


    routes.append(
        infoRoute
    );


    Json::Value evaluateRoute;

    evaluateRoute["method"] =
        "POST";

    evaluateRoute["path"] =
        "/api/evaluate";

    evaluateRoute["description"] =
        "Evaluate a scientific expression";


    routes.append(
        evaluateRoute
    );


    Json::Value programmerRoute;

    programmerRoute["method"] =
        "POST";

    programmerRoute["path"] =
        "/api/programmer";

    programmerRoute["description"] =
        "Perform programmer calculations and conversions";


    routes.append(
        programmerRoute
    );


    json["routes"] =
        routes;


    // ========================================================
    // RUNTIME
    // ========================================================

    Json::Value runtime;


    runtime["containerized"] =
        true;


    runtime["container"] =
        "Docker";


    runtime["buildSystem"] =
        "CMake";


    runtime["ci"] =
        "GitHub Actions";


    runtime["overflowPolicy"] =
        "wrap-to-selected-width";


    json["runtime"] =
        runtime;


    return json;
}


// ============================================================
// MAIN
// ============================================================

int main()
{
    // Apply the same browser security baseline to the homepage, static assets,
    // API responses and errors. Keeping this in one post-handling hook prevents
    // individual routes from accidentally omitting a header.
    app().registerPostHandlingAdvice(
        [](const HttpRequestPtr &,
           const HttpResponsePtr &response)
        {
            response->addHeader(
                "Content-Security-Policy",
                "default-src 'self'; base-uri 'self'; object-src 'none'; "
                "frame-ancestors 'none'; form-action 'self'; "
                "script-src 'self'; style-src 'self'; "
                "img-src 'self' data:; font-src 'self'; "
                "connect-src 'self'; frame-src 'none'; worker-src 'self'; "
                "manifest-src 'self'; upgrade-insecure-requests"
            );
            response->addHeader(
                "Strict-Transport-Security",
                "max-age=63072000; includeSubDomains; preload"
            );
            response->addHeader(
                "Referrer-Policy",
                "no-referrer"
            );
            response->addHeader(
                "Permissions-Policy",
                "accelerometer=(), autoplay=(), camera=(), geolocation=(), "
                "gyroscope=(), magnetometer=(), microphone=(), payment=(), "
                "usb=()"
            );
            response->addHeader(
                "X-Content-Type-Options",
                "nosniff"
            );
            response->addHeader(
                "X-Frame-Options",
                "DENY"
            );
            response->addHeader(
                "Cross-Origin-Opener-Policy",
                "same-origin"
            );
            response->addHeader(
                "Cross-Origin-Resource-Policy",
                "same-origin"
            );
            response->addHeader(
                "X-Permitted-Cross-Domain-Policies",
                "none"
            );
        }
    );

    // ========================================================
    // STATIC FRONTEND
    // ========================================================

    app().registerHandler(
        "/",

        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            callback(
                HttpResponse::newFileResponse(
                    "/app/frontend/index.html"
                )
            );
        },

        {
            Get
        }
    );


    app().registerHandler(
        "/style.css",

        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            auto response =
                HttpResponse::newFileResponse(
                    "/app/frontend/style.css"
                );


            response->setContentTypeCode(
                CT_TEXT_CSS
            );


            callback(
                response
            );
        },

        {
            Get
        }
    );


    app().registerHandler(
        "/app.js",

        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            auto response =
                HttpResponse::newFileResponse(
                    "/app/frontend/app.js"
                );


            response->setContentTypeCode(
                CT_TEXT_JAVASCRIPT
            );


            callback(
                response
            );
        },

        {
            Get
        }
    );


    // ========================================================
    // HEALTH ENDPOINT
    // ========================================================

    app().registerHandler(
        "/health",

        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            Json::Value json;


            json["status"] =
                "ok";


            json["service"] =
                "scientific-calculator-cpp";


            json["name"] =
                APPLICATION_NAME;


            json["version"] =
                APPLICATION_VERSION;


            json["language"] =
                APPLICATION_LANGUAGE;


            json["framework"] =
                APPLICATION_FRAMEWORK;


            json["engine"] =
                "cpp-expression-parser";


            json["scientificMode"] =
                true;


            json["programmerMode"] =
                true;


            json["programmerWidths"] =
                "8,16,32,64";


            json["signedProgrammerMode"] =
                true;


            json["rotateOperations"] =
                "ROL,ROR";


            json["interactiveBits"] =
                true;


            json["angleModes"] =
                "DEG,RAD";


            json["programmerBases"] =
                "BIN,OCT,DEC,HEX";


            json["apiInfo"] =
                "/api/info";


            callback(
                jsonResponse(
                    json
                )
            );
        },

        {
            Get
        }
    );


    // ========================================================
    // API INFORMATION ENDPOINT
    // ========================================================

    app().registerHandler(
        "/api/info",

        [](const HttpRequestPtr &,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            callback(
                jsonResponse(
                    buildApiInfo()
                )
            );
        },

        {
            Get
        }
    );


    // ========================================================
    // SCIENTIFIC API
    // ========================================================

    app().registerHandler(
        "/api/evaluate",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            if (
                req->method() ==
                Options
            )
            {
                Json::Value json;


                json["status"] =
                    "ok";


                callback(
                    jsonResponse(
                        json
                    )
                );


                return;
            }


            auto json =
                req->getJsonObject();


            if (!json)
            {
                callback(
                    errorResponse(
                        "Invalid JSON request."
                    )
                );


                return;
            }


            if (
                !json->isMember(
                    "expression"
                )
            )
            {
                callback(
                    errorResponse(
                        "Missing expression."
                    )
                );


                return;
            }


            const std::string input =
                (*json)["expression"]
                    .asString();


            if (input.empty())
            {
                callback(
                    errorResponse(
                        "Expression cannot be empty."
                    )
                );


                return;
            }


            if (
                input.length() >
                500
            )
            {
                callback(
                    errorResponse(
                        "Expression is too long."
                    )
                );


                return;
            }


            std::string requestedMode =
                "DEG";


            if (
                json->isMember(
                    "mode"
                )
            )
            {
                requestedMode =
                    (*json)["mode"]
                        .asString();
            }


            try
            {
                const AngleMode mode =
                    parseAngleMode(
                        requestedMode
                    );


                ExpressionParser parser(
                    input,
                    mode
                );


                const double result =
                    parser.parse();


                Json::Value responseJson;


                responseJson["expression"] =
                    input;


                responseJson["mode"] =
                    angleModeToString(
                        mode
                    );


                responseJson["result"] =
                    result;


                callback(
                    jsonResponse(
                        responseJson
                    )
                );
            }

            catch (
                const std::exception &error
            )
            {
                callback(
                    errorResponse(
                        error.what()
                    )
                );
            }
        },

        {
            Post,
            Options
        }
    );


    // ========================================================
    // PROGRAMMER API
    // ========================================================

    app().registerHandler(
        "/api/programmer",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            if (
                req->method() ==
                Options
            )
            {
                Json::Value json;


                json["status"] =
                    "ok";


                callback(
                    jsonResponse(
                        json
                    )
                );


                return;
            }


            auto json =
                req->getJsonObject();


            if (!json)
            {
                callback(
                    errorResponse(
                        "Invalid JSON request."
                    )
                );


                return;
            }


            if (
                !json->isMember(
                    "operation"
                ) ||
                !json->isMember(
                    "left"
                ) ||
                !json->isMember(
                    "base"
                )
            )
            {
                callback(
                    errorResponse(
                        "Programmer request requires operation, left and base."
                    )
                );


                return;
            }


            try
            {
                std::string operation =
                    (*json)["operation"]
                        .asString();


                std::transform(
                    operation.begin(),
                    operation.end(),
                    operation.begin(),
                    [](unsigned char character)
                    {
                        return static_cast<char>(
                            std::toupper(character)
                        );
                    }
                );


                const std::string baseName =
                    (*json)["base"]
                        .asString();


                const int base =
                    programmerBaseFromName(
                        baseName
                    );


                unsigned int width =
                    64;


                if (
                    json->isMember(
                        "width"
                    )
                )
                {
                    width =
                        (*json)["width"]
                            .asUInt();
                }


                width =
                    parseBitWidth(
                        width
                    );


                bool signedMode =
                    false;


                if (
                    json->isMember(
                        "signed"
                    )
                )
                {
                    signedMode =
                        (*json)["signed"]
                            .asBool();
                }


                const uint64_t mask =
                    maskForWidth(
                        width
                    );


                const uint64_t left =
                    parseProgrammerValue(
                        (*json)["left"]
                            .asString(),
                        base,
                        width,
                        signedMode
                    );


                uint64_t result =
                    left;


                // =================================================
                // CONVERT
                // =================================================

                if (
                    operation ==
                    "CONVERT"
                )
                {
                    result =
                        left;
                }


                // =================================================
                // NOT
                // =================================================

                else if (
                    operation ==
                    "NOT"
                )
                {
                    result =
                        (
                            ~left
                        ) &
                        mask;
                }


                // =================================================
                // TOGGLE BIT
                // =================================================

                else if (
                    operation ==
                    "TOGGLE"
                )
                {
                    if (
                        !json->isMember(
                            "right"
                        )
                    )
                    {
                        throw std::runtime_error(
                            "TOGGLE requires a bit index."
                        );
                    }


                    const unsigned int index =
                        static_cast<unsigned int>(
                            std::stoul(
                                (*json)["right"]
                                    .asString()
                            )
                        );


                    if (index >= width)
                    {
                        throw std::runtime_error(
                            "Bit index is outside the selected width."
                        );
                    }


                    result =
                        left ^
                        (
                            uint64_t{1} <<
                            index
                        );
                }


                // =================================================
                // BINARY OPERATIONS
                // =================================================

                else
                {
                    if (
                        !json->isMember(
                            "right"
                        )
                    )
                    {
                        throw std::runtime_error(
                            "This programmer operation requires a right value."
                        );
                    }


                    // =============================================
                    // SHIFT / ROTATION
                    // =============================================

                    if (
                        operation == "SHL" ||
                        operation == "SHR" ||
                        operation == "ROL" ||
                        operation == "ROR"
                    )
                    {
                        const unsigned int amount =
                            static_cast<unsigned int>(
                                std::stoul(
                                    (*json)["right"]
                                        .asString()
                                )
                            );


                        if (
                            operation ==
                            "SHL"
                        )
                        {
                            if (amount >= width)
                            {
                                result =
                                    0;
                            }

                            else
                            {
                                result =
                                    (
                                        left <<
                                        amount
                                    ) &
                                    mask;
                            }
                        }


                        else if (
                            operation ==
                            "SHR"
                        )
                        {
                            if (amount >= width)
                            {
                                if (
                                    signedMode &&
                                    (
                                        left &
                                        signBitForWidth(
                                            width
                                        )
                                    )
                                )
                                {
                                    result =
                                        mask;
                                }

                                else
                                {
                                    result =
                                        0;
                                }
                            }

                            else if (signedMode)
                            {
                                result =
                                    arithmeticShiftRight(
                                        left,
                                        amount,
                                        width
                                    );
                            }

                            else
                            {
                                result =
                                    left >>
                                    amount;
                            }
                        }


                        else if (
                            operation ==
                            "ROL"
                        )
                        {
                            result =
                                rotateLeft(
                                    left,
                                    amount,
                                    width
                                );
                        }


                        else
                        {
                            result =
                                rotateRight(
                                    left,
                                    amount,
                                    width
                                );
                        }
                    }


                    // =============================================
                    // AND / OR / XOR
                    // =============================================

                    else
                    {
                        const uint64_t right =
                            parseProgrammerValue(
                                (*json)["right"]
                                    .asString(),
                                base,
                                width,
                                signedMode
                            );


                        if (
                            operation ==
                            "AND"
                        )
                        {
                            result =
                                left &
                                right;
                        }


                        else if (
                            operation ==
                            "OR"
                        )
                        {
                            result =
                                left |
                                right;
                        }


                        else if (
                            operation ==
                            "XOR"
                        )
                        {
                            result =
                                left ^
                                right;
                        }


                        else
                        {
                            throw std::runtime_error(
                                "Unsupported programmer operation."
                            );
                        }
                    }
                }


                Json::Value responseJson =
                    programmerResultJson(
                        result,
                        width,
                        signedMode
                    );


                responseJson["operation"] =
                    operation;


                responseJson["base"] =
                    baseName;


                responseJson["engineVersion"] =
                    APPLICATION_VERSION;


                callback(
                    jsonResponse(
                        responseJson
                    )
                );
            }

            catch (
                const std::exception &error
            )
            {
                callback(
                    errorResponse(
                        error.what()
                    )
                );
            }
        },

        {
            Post,
            Options
        }
    );


    // ========================================================
    // SERVER
    // ========================================================

    const int port =
        getServerPort();


    std::cout
        << APPLICATION_NAME
        << " v"
        << APPLICATION_VERSION
        << std::endl;


    std::cout
        << "Scientific expression engine enabled"
        << std::endl;


    std::cout
        << "Advanced programmer engine enabled"
        << std::endl;


    std::cout
        << "API metadata available at /api/info"
        << std::endl;


    std::cout
        << "Listening on port "
        << port
        << std::endl;


    app()
        .addListener(
            "0.0.0.0",
            port
        )
        .setThreadNum(
            2
        )
        .run();


    return 0;
}
