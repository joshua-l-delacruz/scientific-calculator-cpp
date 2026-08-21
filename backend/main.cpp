#include <drogon/drogon.h>
#include <cmath>
#include <string>

using namespace drogon;

constexpr double PI = 3.14159265358979323846;

HttpResponsePtr jsonResponse(
    const Json::Value &json,
    HttpStatusCode status = k200OK)
{
    auto response =
        HttpResponse::newHttpJsonResponse(json);

    response->setStatusCode(status);

    // Allow the frontend to communicate with the API
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
        "POST, OPTIONS"
    );

    return response;
}

int main()
{
    app().registerHandler(
        "/api/calculate",

        [](const HttpRequestPtr &req,
           std::function<void(const HttpResponsePtr &)> &&callback)
        {
            // Handle browser CORS preflight request
            if (req->method() == Options)
            {
                Json::Value empty;

                callback(
                    jsonResponse(empty)
                );

                return;
            }

            auto json =
                req->getJsonObject();

            if (!json)
            {
                Json::Value error;

                error["error"] =
                    "Invalid JSON request.";

                callback(
                    jsonResponse(
                        error,
                        k400BadRequest
                    )
                );

                return;
            }

            if (!json->isMember("operation"))
            {
                Json::Value error;

                error["error"] =
                    "Missing operation.";

                callback(
                    jsonResponse(
                        error,
                        k400BadRequest
                    )
                );

                return;
            }

            std::string operation =
                (*json)["operation"].asString();

            double result = 0.0;

            // =====================================
            // Binary operations
            // =====================================

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
                    Json::Value error;

                    error["error"] =
                        "Missing left or right value.";

                    callback(
                        jsonResponse(
                            error,
                            k400BadRequest
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
                        Json::Value error;

                        error["error"] =
                            "Cannot divide by zero.";

                        callback(
                            jsonResponse(
                                error,
                                k400BadRequest
                            )
                        );

                        return;
                    }

                    result =
                        left / right;
                }
            }

            // =====================================
            // Scientific unary operations
            // =====================================

            else
            {
                if (!json->isMember("value"))
                {
                    Json::Value error;

                    error["error"] =
                        "Missing value.";

                    callback(
                        jsonResponse(
                            error,
                            k400BadRequest
                        )
                    );

                    return;
                }

                double value =
                    (*json)["value"].asDouble();

                if (operation == "sqrt")
                {
                    if (value < 0)
                    {
                        Json::Value error;

                        error["error"] =
                            "Square root requires a non-negative number.";

                        callback(
                            jsonResponse(
                                error,
                                k400BadRequest
                            )
                        );

                        return;
                    }

                    result =
                        std::sqrt(value);
                }

                else if (operation == "sin")
                {
                    double radians =
                        value * PI / 180.0;

                    result =
                        std::sin(radians);
                }

                else if (operation == "cos")
                {
                    double radians =
                        value * PI / 180.0;

                    result =
                        std::cos(radians);
                }

                else if (operation == "tan")
                {
                    double radians =
                        value * PI / 180.0;

                    result =
                        std::tan(radians);
                }

                else if (operation == "log")
                {
                    if (value <= 0)
                    {
                        Json::Value error;

                        error["error"] =
                            "Logarithm requires a positive number.";

                        callback(
                            jsonResponse(
                                error,
                                k400BadRequest
                            )
                        );

                        return;
                    }

                    result =
                        std::log10(value);
                }

                else if (operation == "ln")
                {
                    if (value <= 0)
                    {
                        Json::Value error;

                        error["error"] =
                            "Natural logarithm requires a positive number.";

                        callback(
                            jsonResponse(
                                error,
                                k400BadRequest
                            )
                        );

                        return;
                    }

                    result =
                        std::log(value);
                }

                else if (operation == "negate")
                {
                    result =
                        -value;
                }

                else
                {
                    Json::Value error;

                    error["error"] =
                        "Unsupported calculator operation.";

                    callback(
                        jsonResponse(
                            error,
                            k400BadRequest
                        )
                    );

                    return;
                }
            }

            Json::Value responseJson;

            responseJson["operation"] =
                operation;

            responseJson["result"] =
                result;

            callback(
                jsonResponse(responseJson)
            );
        },

        {
            Post,
            Options
        }
    );

    app()
        .addListener(
            "0.0.0.0",
            8080
        )
        .run();
}
