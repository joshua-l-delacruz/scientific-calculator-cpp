#include <drogon/drogon.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace drogon;

constexpr double PI =
    3.141592653589793238462643383279502884;

constexpr double EULER_E =
    2.718281828459045235360287471352662498;


HttpResponsePtr jsonResponse(
    const Json::Value &json,
    HttpStatusCode status = k200OK)
{
    auto response =
        HttpResponse::newHttpJsonResponse(json);

    response->setStatusCode(status);

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
    HttpStatusCode status = k400BadRequest)
{
    Json::Value json;

    json["error"] =
        message;

    return jsonResponse(
        json,
        status
    );
}


int getServerPort()
{
    const char *portEnvironment =
        std::getenv("PORT");

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


double factorial(int number)
{
    double result =
        1.0;

    for (
        int i = 2;
        i <= number;
        ++i
    )
    {
        result *=
            static_cast<double>(i);
    }

    return result;
}


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

            callback(response);
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

            callback(response);
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

            callback(response);
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
            Json::Value responseJson;

            responseJson["status"] =
                "ok";

            responseJson["service"] =
                "scientific-calculator-cpp";

            responseJson["version"] =
                "2.0";

            callback(
                jsonResponse(
                    responseJson
                )
            );
        },

        {
            Get
        }
    );


    // ========================================================
    // CALCULATOR API
    // ========================================================

    app().registerHandler(
        "/api/calculate",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            if (req->method() == Options)
            {
                Json::Value responseJson;

                responseJson["status"] =
                    "ok";

                callback(
                    jsonResponse(
                        responseJson
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


            if (!json->isMember("operation"))
            {
                callback(
                    errorResponse(
                        "Missing operation."
                    )
                );

                return;
            }


            const std::string operation =
                (*json)["operation"].asString();


            double result =
                0.0;


            // =================================================
            // CONSTANTS
            // =================================================

            if (operation == "pi")
            {
                result =
                    PI;
            }

            else if (operation == "e")
            {
                result =
                    EULER_E;
            }


            // =================================================
            // BINARY OPERATIONS
            // =================================================

            else if (
                operation == "add" ||
                operation == "subtract" ||
                operation == "multiply" ||
                operation == "divide" ||
                operation == "power"
            )
            {
                if (
                    !json->isMember("left") ||
                    !json->isMember("right")
                )
                {
                    callback(
                        errorResponse(
                            "Missing left or right value."
                        )
                    );

                    return;
                }


                const double left =
                    (*json)["left"].asDouble();

                const double right =
                    (*json)["right"].asDouble();


                if (operation == "add")
                {
                    result =
                        left + right;
                }

                else if (operation == "subtract")
                {
                    result =
                        left - right;
                }

                else if (operation == "multiply")
                {
                    result =
                        left * right;
                }

                else if (operation == "divide")
                {
                    if (right == 0.0)
                    {
                        callback(
                            errorResponse(
                                "Cannot divide by zero."
                            )
                        );

                        return;
                    }

                    result =
                        left / right;
                }

                else if (operation == "power")
                {
                    result =
                        std::pow(
                            left,
                            right
                        );
                }
            }


            // =================================================
            // UNARY OPERATIONS
            // =================================================

            else
            {
                if (!json->isMember("value"))
                {
                    callback(
                        errorResponse(
                            "Missing value."
                        )
                    );

                    return;
                }


                const double value =
                    (*json)["value"].asDouble();


                if (operation == "sqrt")
                {
                    if (value < 0.0)
                    {
                        callback(
                            errorResponse(
                                "Square root requires a non-negative number."
                            )
                        );

                        return;
                    }

                    result =
                        std::sqrt(value);
                }

                else if (operation == "square")
                {
                    result =
                        value * value;
                }

                else if (operation == "cube")
                {
                    result =
                        value * value * value;
                }

                else if (operation == "reciprocal")
                {
                    if (value == 0.0)
                    {
                        callback(
                            errorResponse(
                                "Cannot calculate the reciprocal of zero."
                            )
                        );

                        return;
                    }

                    result =
                        1.0 / value;
                }

                else if (operation == "percent")
                {
                    result =
                        value / 100.0;
                }

                else if (operation == "factorial")
                {
                    if (value < 0.0)
                    {
                        callback(
                            errorResponse(
                                "Factorial requires a non-negative integer."
                            )
                        );

                        return;
                    }


                    const double rounded =
                        std::round(value);


                    if (
                        std::abs(
                            value - rounded
                        ) > 1e-12
                    )
                    {
                        callback(
                            errorResponse(
                                "Factorial requires a whole number."
                            )
                        );

                        return;
                    }


                    if (rounded > 170.0)
                    {
                        callback(
                            errorResponse(
                                "Factorial is limited to 170 to avoid overflow."
                            )
                        );

                        return;
                    }


                    result =
                        factorial(
                            static_cast<int>(
                                rounded
                            )
                        );
                }

                else if (operation == "sin")
                {
                    const double radians =
                        value *
                        PI /
                        180.0;

                    result =
                        std::sin(
                            radians
                        );
                }

                else if (operation == "cos")
                {
                    const double radians =
                        value *
                        PI /
                        180.0;

                    result =
                        std::cos(
                            radians
                        );
                }

                else if (operation == "tan")
                {
                    const double radians =
                        value *
                        PI /
                        180.0;


                    const double cosine =
                        std::cos(
                            radians
                        );


                    if (
                        std::abs(cosine) <
                        1e-12
                    )
                    {
                        callback(
                            errorResponse(
                                "Tangent is undefined for this angle."
                            )
                        );

                        return;
                    }


                    result =
                        std::tan(
                            radians
                        );
                }

                else if (operation == "log")
                {
                    if (value <= 0.0)
                    {
                        callback(
                            errorResponse(
                                "Logarithm requires a positive number."
                            )
                        );

                        return;
                    }


                    result =
                        std::log10(
                            value
                        );
                }

                else if (operation == "ln")
                {
                    if (value <= 0.0)
                    {
                        callback(
                            errorResponse(
                                "Natural logarithm requires a positive number."
                            )
                        );

                        return;
                    }


                    result =
                        std::log(
                            value
                        );
                }

                else if (operation == "negate")
                {
                    result =
                        -value;
                }

                else
                {
                    callback(
                        errorResponse(
                            "Unsupported calculator operation."
                        )
                    );

                    return;
                }
            }


            if (!std::isfinite(result))
            {
                callback(
                    errorResponse(
                        "The calculation produced an invalid or overflowing result."
                    )
                );

                return;
            }


            Json::Value responseJson;

            responseJson["operation"] =
                operation;

            responseJson["result"] =
                result;


            callback(
                jsonResponse(
                    responseJson
                )
            );
        },

        {
            Post,
            Options
        }
    );


    const int port =
        getServerPort();


    std::cout
        << "C++ Scientific Calculator v2.0"
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
