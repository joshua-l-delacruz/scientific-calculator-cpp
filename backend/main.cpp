#include <drogon/drogon.h>

#include <algorithm>
#include <bitset>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

using namespace drogon;


// ============================================================
// CONSTANTS
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
// PORT
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
            << "Using 8080."
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


        const std::string numberText =
            expression.substr(
                start,
                position - start
            );


        try
        {
            return std::stod(
                numberText
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
        if (
            angleMode ==
            AngleMode::RAD
        )
        {
            return value;
        }


        return value *
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
// PROGRAMMER MODE HELPERS
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
                return std::isspace(character);
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


uint64_t parseUnsignedInteger(
    std::string text,
    int base
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


    if (
        base == 16 &&
        text.rfind("0X", 0) == 0
    )
    {
        text =
            text.substr(2);
    }


    if (
        base == 2 &&
        text.rfind("0B", 0) == 0
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


    std::size_t consumed =
        0;


    try
    {
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
            "Value exceeds the 64-bit unsigned integer range."
        );
    }
}


std::string unsignedToBase(
    uint64_t value,
    int base
)
{
    if (base == 10)
    {
        return std::to_string(
            value
        );
    }


    if (value == 0)
    {
        return "0";
    }


    const char digits[] =
        "0123456789ABCDEF";


    std::string result;


    while (value > 0)
    {
        result.push_back(
            digits[
                value %
                static_cast<uint64_t>(base)
            ]
        );


        value /=
            static_cast<uint64_t>(base);
    }


    std::reverse(
        result.begin(),
        result.end()
    );


    return result;
}


Json::Value programmerResultJson(
    uint64_t result
)
{
    Json::Value json;


    json["result"] =
        std::to_string(result);


    json["bin"] =
        unsignedToBase(
            result,
            2
        );


    json["oct"] =
        unsignedToBase(
            result,
            8
        );


    json["dec"] =
        unsignedToBase(
            result,
            10
        );


    json["hex"] =
        unsignedToBase(
            result,
            16
        );


    return json;
}


// ============================================================
// MAIN
// ============================================================

int main()
{
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
    // HEALTH
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


            json["version"] =
                "7.0";


            json["engine"] =
                "cpp-expression-parser";


            json["programmerMode"] =
                true;


            json["programmerIntegerSize"] =
                "64-bit unsigned";


            json["angleModes"] =
                "DEG,RAD";


            json["programmerBases"] =
                "BIN,OCT,DEC,HEX";


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


                const uint64_t left =
                    parseUnsignedInteger(
                        (*json)["left"]
                            .asString(),
                        base
                    );


                uint64_t result =
                    left;


                if (operation == "CONVERT")
                {
                    result =
                        left;
                }

                else if (operation == "NOT")
                {
                    result =
                        ~left;
                }

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


                    const uint64_t right =
                        parseUnsignedInteger(
                            (*json)["right"]
                                .asString(),
                            base
                        );


                    if (operation == "AND")
                    {
                        result =
                            left & right;
                    }

                    else if (operation == "OR")
                    {
                        result =
                            left | right;
                    }

                    else if (operation == "XOR")
                    {
                        result =
                            left ^ right;
                    }

                    else if (operation == "SHL")
                    {
                        if (right > 63)
                        {
                            throw std::runtime_error(
                                "Shift amount must be between 0 and 63."
                            );
                        }


                        result =
                            left <<
                            static_cast<unsigned int>(
                                right
                            );
                    }

                    else if (operation == "SHR")
                    {
                        if (right > 63)
                        {
                            throw std::runtime_error(
                                "Shift amount must be between 0 and 63."
                            );
                        }


                        result =
                            left >>
                            static_cast<unsigned int>(
                                right
                            );
                    }

                    else
                    {
                        throw std::runtime_error(
                            "Unsupported programmer operation."
                        );
                    }
                }


                Json::Value responseJson =
                    programmerResultJson(
                        result
                    );


                responseJson["operation"] =
                    operation;


                responseJson["base"] =
                    baseName;


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
        << "C++ Scientific Calculator v7.0"
        << std::endl;


    std::cout
        << "Scientific + Programmer modes enabled"
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
        .setThreadNum(2)
        .run();


    return 0;
}
