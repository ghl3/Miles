
#include "crow.h"

int main() {

    crow::SimpleApp app;

    app.route_dynamic("/")
            ([]() {
                return "Hello world";
            });

    app.route_dynamic("/hello/<int>")
            ([](int count) {
                if (count > 100) {
                    return crow::response(400);
                } else {
                    std::ostringstream os;
                    os << count << " bottles of beer!";
                    return crow::response(os.str());
                }
            });

    app.route_dynamic("/add_json").methods("POST"_method)
            ([](const crow::request &req) {
                auto x = crow::json::load(req.body);
                if (!x) {
                    return crow::response(400);
                } else {
                    int64_t sum = x["a"].i() + x["b"].i();
                    std::ostringstream os;
                    os << sum;
                    return crow::response{os.str()};
                }
            });

    app.port(18080).multithreaded().run();
}
