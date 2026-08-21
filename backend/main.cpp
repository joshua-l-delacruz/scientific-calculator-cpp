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
            << "Using 8080 instead."
            << std::endl;

        return 8080;
    }
}


// ============================================================
// MAIN APPLICATION
// ============================================================

int main()
{
    // --------------------------------------------------------
    // FRONTEND
    // --------------------------------------------------------

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
                CT_APPLICATION_JAVASCRIPT
            );

            callback(response);
        },

        {
            Get
        }
    );


    // --------------------------------------------------------
    // HEALTH CHECK
    // --------------------------------------------------------

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


    // --------------------------------------------------------
    // CALCULATOR API
    // --------------------------------------------------------

    app().registerHandler(
        "/api/calculate",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            // ------------------------------------------------
            // CORS PREFLIGHT
            // ------------------------------------------------

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


            // ------------------------------------------------
            // PARSE JSON
            // ------------------------------------------------

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


            std::string operation =
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


                double left =
                    (*json)["left"].asDouble();

                double right =
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


                double value =
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
                    double radians =
                        value * PI / 180.0;

                    result =
                        std::sin(radians);
                }


                // ---------------------------------------------
                // COSINE
                // ---------------------------------------------

                else if (operation == "cos")
                {
                    double radians =
                        value * PI / 180.0;

                    result =
                        std::cos(radians);
                }


                // ---------------------------------------------
                // TANGENT
                // ---------------------------------------------

                else if (operation == "tan")
                {
                    double radians =
                        value * PI / 180.0;

                    double cosine =
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
                // BASE-10 LOG
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
            // RESULT VALIDATION
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
            // SUCCESSFUL RESPONSE
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


    // =========================================================
    // SERVER CONFIGURATION
    // =========================================================

    const int port =
        getServerPort();

    std::cout
        << "Starting C++ Scientific Calculator"
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
