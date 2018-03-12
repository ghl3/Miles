


#include <string>

#include "json.h"
#include "crow.h"

using json = nlohmann::json;



int main() {

    crow::SimpleApp app;

    app.route_dynamic("/")
            ([]() {
                return "Hello world";
            });



    app.route_dynamic("/store/<str>/<str>").methods("POST"_method)
            ([](const crow::request &req, const std::string table, const std::string key) {

                try {

                    // Is this a copy or a move?
                    const json& payload = json::parse(req.body);

                    std::cout << payload << std::endl;

                    return crow::response(200, "Successfully stored key");


                } catch (const json::parse_error& e) {
                    return crow::response(404, "Unable to parse JSON");
                }
                /*
                @throw parse_error.101 if a parse error occurs; example: `""unexpected end
                of input; expected string literal""`
                @throw parse_error.102 if to_unicode fails or surrogate error
                @throw parse_error.103 if to_unicode fails
                 */

                //if (!x)
                //    return crow::response(400);
                //int sum = x["a"].i()+x["b"].i();






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
