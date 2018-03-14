

#include <utility>
#include <string>
#include <storage.h>

#include "json.h"
#include "crow.h"

using json = nlohmann::json;


std::unique_ptr<crow::SimpleApp> createServer(const std::shared_ptr<IStorage>& storage) {

    auto app = std::make_unique<crow::SimpleApp>();

    app->route_dynamic("/")
            ([]() {
                return "Hello world";
            });

    app->route_dynamic("/store/<str>/<str>").methods("POST"_method)
            ([=](const crow::request &req, const std::string table, const std::string key) {

                 try {

                     auto payload = std::make_unique<json>(json::parse(req.body));
                     std::cout << *payload << std::endl;
                     auto storageResult = storage->store(table, key, std::move(payload));
                     std::cout << "Did we store successfully: " << storageResult.success << std::endl;
                     return crow::response(200, "Successfully stored key");

                 } catch (const json::parse_error &e) {
                     return crow::response(404, "Unable to parse JSON");
                 }
             });

    app->route_dynamic("/fetch/<str>/<str>")
            ([=](const std::string table, const std::string key) {

                auto fetchResult = storage->fetch(table, key);
                std::cout << "Did we fetch successfully: " << fetchResult.success << std::endl;

                if (fetchResult.success) {
                    return crow::response(200, fetchResult.getJson().dump());
                } else {
                    return crow::response(404);
                }
            });

    app->route_dynamic("/hello/<int>")
            ([](
                    int count
            ) {
                if (count > 100) {
                    return crow::response(400);
                } else {
                    std::ostringstream os;
                    os << count << " bottles of beer!";
                    return crow::response(os.str());
                }
            });

    app->route_dynamic("/add_json").methods("POST"_method)
            ([](
                    const crow::request &req
            ) {
                auto x = crow::json::load(req.body);
                if (!x) {
                    return crow::response(400);
                } else {
                    int64_t sum = x["a"].i() + x["b"].i();
                    std::ostringstream os;
                    os << sum;
                    return crow::response(os.str());
                }
            });

    return app;

}


int main() {

    // Initialize the Sorage Engine
    std::unique_ptr<IStorage> storage = std::make_unique<MapStorage>();

    // Initialize the webserver
    auto app = createServer(std::move(storage));

    (*app).port(18080).multithreaded().run();
}
