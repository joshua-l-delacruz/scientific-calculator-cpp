#include <drogon/drogon.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>

using namespace drogon;

constexpr double PI =
    3.14159265358979323846;


// ============================================================
// JSON RESPONSE HELPER
// ============================================================

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


// ============================================================
// ERROR RESPONSE HELPER
// ============================================================

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


// ============================================================
// SERVER PORT
// ============================================================

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


// ============================================================
// MAIN
// ============================================================

int main()
{
    // ========================================================
    // FRONTEND — INDEX.HTML
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


    // ========================================================
    // FRONTEND — STYLE.CSS
    // ========================================================

    app().registerHandler(
        "/style.css",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            auto response =
                HttpResponse::newFileResponse(
                    "/app/frontend/style.css"
                );

            callback(response);
        },

        {
            Get
        }
    );


    // ========================================================
    // FRONTEND — APP.JS
    // ========================================================

    app().registerHandler(
        "/app.js",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            auto response =
                HttpResponse::newFileResponse(
                    "/app/frontend/app.js"
                );

            callback(response);
        },

        {
            Get
        }
    );


    // ========================================================
    // HEALTH CHECK
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
                "1.1";

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
            // =================================================
            // OPTIONS / CORS
            // =================================================

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


            // =================================================
            // READ JSON REQUEST
            // =================================================

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
            // BINARY OPERATIONS
            // =================================================

            if (
                operation == "add" ||
                operation == "subtract" ||
                operation == "multiply" ||
                operation == "divide"
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


                // ---------------------------------------------
                // ADDITION
                // ---------------------------------------------

                if (operation == "add")
                {
                    result =
                        left + right;
                }


                // ---------------------------------------------
                // SUBTRACTION
                // ---------------------------------------------

                else if (operation == "subtract")
                {
                    result =
                        left - right;
                }


                // ---------------------------------------------
                // MULTIPLICATION
                // ---------------------------------------------

                else if (operation == "multiply")
                {
                    result =
                        left * right;
                }


                // ---------------------------------------------
                // DIVISION
                // ---------------------------------------------

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
            }


            // =================================================
            // SCIENTIFIC / UNARY OPERATIONS
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


                // ---------------------------------------------
                // SQUARE ROOT
                // ---------------------------------------------

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


                // ---------------------------------------------
                // SINE
                // ---------------------------------------------

                else if (operation == "sin")
                {
                    const double radians =
                        value * PI / 180.0;


                    result =
                        std::sin(radians);
                }


                // ---------------------------------------------
                // COSINE
                // ---------------------------------------------

                else if (operation == "cos")
                {
                    const double radians =
                        value * PI / 180.0;


                    result =
                        std::cos(radians);
                }


                // ---------------------------------------------
                // TANGENT
                // ---------------------------------------------

                else if (operation == "tan")
                {
                    const double radians =
                        value * PI / 180.0;


                    const double cosine =
                        std::cos(radians);


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
                        std::tan(radians);
                }


                // ---------------------------------------------
                // LOG BASE 10
                // ---------------------------------------------

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
                        std::log10(value);
                }


                // ---------------------------------------------
                // NATURAL LOG
                // ---------------------------------------------

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
                        std::log(value);
                }


                // ---------------------------------------------
                // NEGATE
                // ---------------------------------------------

                else if (operation == "negate")
                {
                    result =
                        -value;
                }


                // ---------------------------------------------
                // UNKNOWN OPERATION
                // ---------------------------------------------

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


            // =================================================
            // VALIDATE RESULT
            // =================================================

            if (!std::isfinite(result))
            {
                callback(
                    errorResponse(
                        "The calculation produced an invalid result."
                    )
                );

                return;
            }


            // =================================================
            // SUCCESS RESPONSE
            // =================================================

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


    // ========================================================
    // START SERVER
    // ========================================================

    const int port =
        getServerPort();


    std::cout
        << "============================================"
        << std::endl;

    std::cout
        << "C++ Scientific Calculator"
        << std::endl;

    std::cout
        << "Drogon server starting on port "
        << port
        << std::endl;

    std::cout
        << "============================================"
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
