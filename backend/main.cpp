#include <drogon/drogon.h>
#include <cmath>
#include <string>

using namespace drogon;

constexpr double PI = 3.14159265358979323846;

int main()
{
    app().registerHandler(
        "/api/calculate",
        [](const HttpRequestPtr& req,
           std::function<void(const HttpResponsePtr&)>&& callback)
        {
            auto json = req->getJsonObject();

            if (!json)
            {
                Json::Value error;
                error["error"] = "Invalid JSON request.";

                auto response =
                    HttpResponse::newHttpJsonResponse(error);

                response->setStatusCode(k400BadRequest);

                callback(response);
                return;
            }

            if (!json->isMember("operation") ||
                !json->isMember("value"))
            {
                Json::Value error;
                error["error"] =
                    "Missing operation or value.";

                auto response =
                    HttpResponse::newHttpJsonResponse(error);

                response->setStatusCode(k400BadRequest);

                callback(response);
                return;
            }

            std::string operation =
                (*json)["operation"].asString();

            double value =
                (*json)["value"].asDouble();

            double result = 0.0;

            if (operation == "sqrt")
            {
                if (value < 0)
                {
                    Json::Value error;
                    error["error"] =
                        "Square root requires a non-negative number.";

                    auto response =
                        HttpResponse::newHttpJsonResponse(error);

                    response->setStatusCode(k400BadRequest);

                    callback(response);
                    return;
                }

                result = std::sqrt(value);
            }
            else if (operation == "sin")
            {
                double radians =
                    value * PI / 180.0;

                result = std::sin(radians);
            }
            else if (operation == "cos")
            {
                double radians =
                    value * PI / 180.0;

                result = std::cos(radians);
            }
            else if (operation == "tan")
            {
                double radians =
                    value * PI / 180.0;

                result = std::tan(radians);
            }
            else if (operation == "log")
            {
                if (value <= 0)
                {
                    Json::Value error;
                    error["error"] =
                        "Logarithm requires a positive number.";

                    auto response =
                        HttpResponse::newHttpJsonResponse(error);

                    response->setStatusCode(k400BadRequest);

                    callback(response);
                    return;
                }

                result = std::log10(value);
            }
            else if (operation == "ln")
            {
                if (value <= 0)
                {
                    Json::Value error;
                    error["error"] =
                        "Natural logarithm requires a positive number.";

                    auto response =
                        HttpResponse::newHttpJsonResponse(error);

                    response->setStatusCode(k400BadRequest);

                    callback(response);
                    return;
                }

                result = std::log(value);
            }
            else
            {
                Json::Value error;
                error["error"] =
                    "Unsupported calculator operation.";

                auto response =
                    HttpResponse::newHttpJsonResponse(error);

                response->setStatusCode(k400BadRequest);

                callback(response);
                return;
            }

            Json::Value responseJson;

            responseJson["operation"] = operation;
            responseJson["input"] = value;
            responseJson["result"] = result;

            callback(
                HttpResponse::newHttpJsonResponse(
                    responseJson
                )
            );
        },
        {Post}
    );

    app()
        .addListener("0.0.0.0", 8080)
        .run();
}
