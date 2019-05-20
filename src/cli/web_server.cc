#include "../utils/id_generator.h"

#include <memory>

#include <crow.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include "web_server.h"
#include "command_option.h"
#include "../core/cli_starter.h"

void WebServer::Start() {
  NewBenchmark();

  app_.loglevel(crow::LogLevel::INFO);
  app_.port(18000).multithreaded().run();
}

/**
 * NewBenchmark -- set route for store new benchmark into buffer
 * Parse the json body from request.
 * If get err, return response(400), set {"status": msg, "error": msg} to description the error
 *         Or, return response(203), set {"id": msg, "location": msg} to mark this benchmark,
 *             and store the body into buffer
 */
void WebServer::NewBenchmark() {
  CROW_ROUTE(app_, "/benchmark")
      .methods("POST"_method)(
          [this](const crow::request &request) {
            rapidjson::Document body;              // For parse the json from request body
            body.SetObject();
            body.Parse(request.body.c_str());

            if (body.HasParseError()) {
              rapidjson::Document err;            // For report error for browser
              err.SetObject();                    // MUST SET OBJECT, This is a big hole!
              err.AddMember("error",
                  rapidjson::StringRef(rapidjson::GetParseError_En(body.GetParseError())),
                  err.GetAllocator());
              err.AddMember("status",
                  rapidjson::Value().SetBool(false),
                  err.GetAllocator());

              rapidjson::StringBuffer buffer;
              rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
              err.Accept(writer);

              crow::response err_resp(400);
              err_resp.write(buffer.GetString());
              return err_resp;
            }

            try {
              auto option = CommandOptions{};
              option.ParseDoc(body);

              crow::response create_resp(200);
              auto optionb = std::move(option.GetOption());
              CliStarter starter{*optionb};
              create_resp.write(starter.GetPrinter());
              return create_resp;
            } catch (std::exception ex){
              crow::response err_resp(400);
              err_resp.write("error: ");
              err_resp.write(ex.what());
              return err_resp;
            }
          }
      );
}