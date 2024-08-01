#include <jwt-cpp/jwt.h>
#include "include/dotenv.h"
#include "Auth.h"
#include "Profile.h"

void Auth::SignUp(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::String imageUrl = (*json)["imageUrl"].asString();
    Json::String name = (*json)["name"].asString();
    std::string email = (*json)["email"].asString();
    Json::String password = (*json)["password"].asString();

    if (imageUrl.empty() || name.empty() || email.empty() || password.empty())
    {
        Json::Value ret;
        ret["message"] = "All fields are required";
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(response);
        return;
    }
    drogon::orm::DbClientPtr app = drogon::app().getDbClient();
    Model::Profile profile(app, email);
    if (profile.ExistsOnDb().success)
    {
        Json::Value ret;
        ret["message"] = "User already exists";
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k409Conflict);
        callback(response);
        return;
    }

    profile.setName(name);
    profile.setPassword(password);
    profile.setImageUrl(imageUrl);
    Model::CrudResponse InsertResponse = profile.InsertInDb();

    if (!InsertResponse.success)
    {
        Json::Value ret;
        ret["message"] = InsertResponse.message;
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(response);
        return;
    }

    Json::Value ret;
    ret["message"] = InsertResponse.message;
    drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
    response->setStatusCode(HttpStatusCode::k201Created);
    callback(response);
    return;
    // auto sqlFuture = app->execSqlAsyncFuture("select * from \"Profile\" where (email = $1)", email);
    // try
    // {
    //     auto result = sqlFuture.get();
    //     for (auto row : result)
    //     {
    //         if (row["email"].as<std::string>() == email)
    //         {
    //             Json::Value ret;
    //             ret["message"] = "User already exists";
    //             drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
    //             response->setStatusCode(HttpStatusCode::k409Conflict);
    //             callback(response);
    //         }
    //     }
    //     std::time_t updatedAt;
    //     std::time(&updatedAt);
    //     std::string updatedAtStr = std::string(ctime(&updatedAt));
    //     auto insertFuture = app->execSqlAsyncFuture("insert into \"Profile\" (\"imageUrl\", name, email, password, \"updatedAt\") values ($1, $2, $3, $4, $5)",
    //                                                 imageUrl,
    //                                                 name,
    //                                                 email,
    //                                                 password,
    //                                                 updatedAtStr);
    //     auto result2 = insertFuture.get();
    //     Json::Value ret;
    //     ret["message"] = "User created";
    //     drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
    //     response->setStatusCode(HttpStatusCode::k201Created);
    //     callback(response);
    // }
    // catch (const std::exception &e)
    // {
    //     std::cout << e.what() << std::endl;
    //     drogon::HttpResponsePtr response = HttpResponse::newHttpResponse();
    //     response->setStatusCode(HttpStatusCode::k500InternalServerError);
    //     callback(response);
    // }
}

void Auth::Login(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::String email = (*json)["email"].asString();
    Json::String password = (*json)["password"].asString();

    std::shared_ptr<orm::DbClient> app = drogon::app().getDbClient();
    auto sqlFuture = app->execSqlAsyncFuture("select * from \"Profile\" where email = $1", email);
    try
    {
        auto result = sqlFuture.get();
        auto row = result[0];
        if (row["email"].as<std::string>() != email)
        {
            Json::Value ret;
            ret["message"] = "User not found";
            drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
            response->setStatusCode(HttpStatusCode::k401Unauthorized);
            callback(response);
        }

        if (row["password"].as<std::string>() != password)
        {
            Json::Value ret;
            ret["message"] = "Invalid password";
            drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
            response->setStatusCode(HttpStatusCode::k401Unauthorized);
            callback(response);
        }

        Json::Value ret;
        auto token = jwt::create()
                         .set_type("JWS")
                         .set_issuer("auth0")
                         .set_expires_in(std::chrono::seconds{36000})
                         .set_payload_claim("email", jwt::claim(email))
                         .set_payload_claim("password", jwt::claim(password))
                         .sign(jwt::algorithm::hs256{dotenv::getenv("ACCESS_SECRET_TOKEN")});
        ret["token"] = token;
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k200OK);
        callback(response);
    }
    catch (const std::exception &e)
    {
        drogon::HttpResponsePtr response = HttpResponse::newHttpResponse();
        response->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(response);
    }
}

void Auth::RefreshToken(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
}

void Auth::DeleteAccount(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback)
{
    std::shared_ptr<Json::Value> json = req->getJsonObject();
    Json::String email = (*json)["email"].asString();

    if (email.empty())
    {
        Json::Value ret;
        ret["message"] = "Email is required";
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k400BadRequest);
        callback(response);
        return;
    }

    drogon::orm::DbClientPtr app = drogon::app().getDbClient();
    Model::Profile profile(app, email);

    if (!profile.ExistsOnDb().success)
    {
        Json::Value ret;
        ret["message"] = "User does not exist";
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k404NotFound);
        callback(response);
        return;
    }

    Model::CrudResponse DeleteResponse = profile.DeleteFromDb();

    if (!DeleteResponse.success)
    {
        Json::Value ret;
        ret["message"] = DeleteResponse.message;
        drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
        response->setStatusCode(HttpStatusCode::k500InternalServerError);
        callback(response);
        return;
    }

    Json::Value ret;
    ret["message"] = DeleteResponse.message;
    drogon::HttpResponsePtr response = HttpResponse::newHttpJsonResponse(ret);
    response->setStatusCode(HttpStatusCode::k200OK);
    callback(response);
    return;
}