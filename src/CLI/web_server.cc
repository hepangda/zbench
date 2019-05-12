/**
 * Created by Crow on 5/9/19.
 * Copyright (c) 2019 Crow All rights reserved.
 * @author Crow 
 */

#include "id_generator.hpp"

#include <memory>

#include <crow.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/error/en.h>

#include "web_server.h"

void WebServer::Start() {
  NewBenchmark();
  SendBenchResult();
  SendBenchmarkRequest();
  ReceiveBenchResult();

  app_.loglevel(crow::LogLevel::INFO);
  app_.port(18000)
      .run();
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

            // create id for benchmark,
            // If you want to extend the response body which stores into buffer, will be sent to benchcore
            // you can use rapidjson::AddMember(), [caller must be Object type]
            auto id = IdGenerator::Instance().Id();
            body.AddMember("id", rapidjson::Value().SetInt64(id), body.GetAllocator());

            // use rapidjson::Writer write DOM into json
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> buf_writer(buffer), res_writer(buffer);

            body.Accept(buf_writer);
            auto json = buffer.GetString();
            requests_.emplace_back(std::make_unique<std::string>(json));        // store the body into buffer

            // If you want to extend the response (to browser)
            // You can use rapidjson::Writer directly,
            // not write DOM into json (slow and high cost)
            buffer.Clear();
            res_writer.StartObject();
            res_writer.Key("status");
            res_writer.Bool(true);
            res_writer.Key("id");
            res_writer.Int64(id);
            res_writer.Key("location");
            res_writer.String(("/benchmark/" + std::to_string(id)).c_str());
            res_writer.EndObject();

            crow::response create_resp(202);                  // Status code 202 means: Async task has been accepted
            create_resp.write(buffer.GetString());            // send {"status": msg, "id": number} to browser

            return create_resp;
          }
      );
}

/**
 * SendBenchResult -- Browser GET the benchmark result.
 * If not find the result, return response(404), with {"status": msg, "error": msg} to describe the condition
 *                     Or, return response(200), with the result from response_.
 */
void WebServer::SendBenchResult() {
  CROW_ROUTE(app_, "/benchmark/<int>")
      .methods("GET"_method)(
          [this](const crow::request &request, long id) {
            std::map<long, std::unique_ptr<std::string>>::iterator iter;
            if ((iter = responses_.find(id)) == responses_.end()) {
              rapidjson::StringBuffer buf;
              rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

              writer.StartObject();
              writer.Key("error");
              writer.String("ID benchmark not found");
              writer.EndObject();

              crow::response err_res(404);
              err_res.write(buf.GetString());

              return err_res;
            }

            crow::response result_resp(200);
            auto res = std::move(responses_[id]);
            responses_.erase(id);
            result_resp.set_header("Content-Type", "application/json");
            result_resp.write(res->c_str());
          }
      );
}

/**
 * ReceiveBenchResult -- get benchmark result from bench core by POST
 * core POST the result marked by ID to http-server
 * If Ok, return response(201) {"status": msg, "id": id} to core
 *    Or, return response(400) {"status": msg, "error": msg, ["id": id]} to core  [] means 可选项
 */
void WebServer::ReceiveBenchResult() {
  CROW_ROUTE(app_, "/benchmark/<int>")
      .methods("POST"_method)(
        [this](const crow::request &request, long id) {
          rapidjson::StringBuffer buf;
          rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

          rapidjson::Document doc;
          doc.Parse(request.body.c_str());

          if (doc.HasParseError()) {                                  // JSON parse error
            crow::response err_resp(400);
            writer.StartObject();
            writer.Key("status");
            writer.Bool(false);
            writer.Key("error");
            writer.String("JSON parses error");
            writer.EndObject();

            err_resp.write(buf.GetString());
            return err_resp;
          }

          if (responses_.find(id) != responses_.cend()) {             // Have gotten the same id benchmark result
            crow::response err_resp(400);

            buf.Clear();
            writer.StartObject();
            writer.Key("status");
            writer.Bool(false);
            writer.Key("error");
            writer.String("same ID benchmark result have been recored");
            writer.Key("id");
            writer.Int64(doc["id"].GetInt64());
            writer.EndObject();

            err_resp.write(buf.GetString());
            return err_resp;
          }

          // Insert the benchmark result into buffer.
          assert(doc["id"].IsInt64());
          responses_.insert({doc["id"].GetInt64(), std::make_unique<std::string>(request.body)});

          rapidjson::Document created_doc;
          created_doc.SetObject();
          created_doc.AddMember("status", rapidjson::Value().SetBool(true), created_doc.GetAllocator());
          created_doc.AddMember("id", rapidjson::Value().SetInt64(doc["id"].GetInt64()), created_doc.GetAllocator());

          buf.Clear();
          created_doc.Accept(writer);

          crow::response success_resp(201);                  // 201 means Created, for POST method
          success_resp.write(buf.GetString());

          return success_resp;
        }
      );
}

/**
 * SendBenchmarkRequest --- bench core get the body by GET
 * If requests_ has bench request, return response(200), means Ok
 *                             Or, return response(404), means not found
 */
void WebServer::SendBenchmarkRequest() {
  CROW_ROUTE(app_, "benchmark")
      .methods("GET"_method) (
        [this]() {
          rapidjson::StringBuffer buf;
          rapidjson::Writer<rapidjson::StringBuffer> writer(buf);

          if (requests_.empty()) {
            crow::response err_resp(404);

            buf.Clear();
            writer.StartObject();
            writer.Key("status");
            writer.Bool(false);
            writer.Key("error");
            writer.String("don't have any request");
            writer.EndObject();

            err_resp.write(buf.GetString());
            return err_resp;
          }

          auto &data = requests_.front();
          requests_.pop_front();

          crow::response benchmark_resp(200);
          benchmark_resp.write(data->c_str());

          return benchmark_resp;
        }
      );
}
