#include <drogon/drogon.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iostream>
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


    if (mode == "RAD")
    {
        return AngleMode::RAD;
    }


    if (mode == "DEG")
    {
        return AngleMode::DEG;
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
// EXPRESSION PARSER
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

    // ========================================================
    // WHITESPACE
    // ========================================================

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


    // ========================================================
    // CHARACTER MATCH
    // ========================================================

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


    // ========================================================
    // EXPRESSION
    //
    // + -
    // ========================================================

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


    // ========================================================
    // TERM
    //
    // * /
    // ========================================================

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

            else
            {
                break;
            }
        }


        return value;
    }


    // ========================================================
    // UNARY
    // ========================================================

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


    // ========================================================
    // POWER
    // ========================================================

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


    // ========================================================
    // POSTFIX
    // ========================================================

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


    // ========================================================
    // PRIMARY
    // ========================================================

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


        // ----------------------------------------------------
        // PARENTHESES
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // NUMBER
        // ----------------------------------------------------

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


        // ----------------------------------------------------
        // IDENTIFIER
        // ----------------------------------------------------

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


    // ========================================================
    // NUMBER
    // ========================================================

    double parseNumber()
    {
        skipWhitespace();


        const std::size_t start =
            position;


        bool decimalFound =
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


        if (
            start ==
            position
        )
        {
            throw std::runtime_error(
                "Expected number."
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


    // ========================================================
    // IDENTIFIER
    // ========================================================

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


    // ========================================================
    // ANGLE CONVERSION
    // ========================================================

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


    // ========================================================
    // FUNCTIONS
    // ========================================================

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


            const double cosine =
                std::cos(
                    radians
                );


            if (
                std::abs(cosine) <
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
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // FRONTEND
    // ========================================================

    app().registerHandler(
        "/",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            auto response =
                HttpResponse::newFileResponse(
                    "/app/frontend/index.html"
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
        "/style.css",

        [](const HttpRequestPtr &req,
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

        [](const HttpRequestPtr &req,
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

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            Json::Value json;


            json["status"] =
                "ok";


            json["service"] =
                "scientific-calculator-cpp";


            json["version"] =
                "5.0";


            json["engine"] =
                "cpp-expression-parser";


            json["angleModes"] =
                "DEG,RAD";


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
    // EXPRESSION API
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
    // SERVER
    // ========================================================

    const int port =
        getServerPort();


    std::cout
        << "C++ Scientific Calculator v5.0"
        << std::endl;


    std::cout
        << "Memory and history frontend enabled"
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
