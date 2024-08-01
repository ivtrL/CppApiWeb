#pragma once

#include <drogon/HttpController.h>
using namespace drogon;
/**
 * @brief this class is created by the drogon_ctl command (drogon_ctl create controller -r Auth).
 * this class is a restful API controller.
 */
class Auth : public drogon::HttpController<Auth>
{
public:
  METHOD_LIST_BEGIN
  METHOD_ADD(Auth::SignUp, "/signup", Post, Options);
  METHOD_ADD(Auth::Login, "/login", Post, Options);
  METHOD_ADD(Auth::RefreshToken, "/refresh-token", Post, Options);
  METHOD_ADD(Auth::DeleteAccount, "/delete-account", Post, Options);
  METHOD_LIST_END

  void SignUp(const HttpRequestPtr &req,
              std::function<void(const HttpResponsePtr &)> &&callback);

  void Login(const HttpRequestPtr &req,
             std::function<void(const HttpResponsePtr &)> &&callback);

  void RefreshToken(const HttpRequestPtr &req,
                    std::function<void(const HttpResponsePtr &)> &&callback);

  void DeleteAccount(const HttpRequestPtr &req,
                     std::function<void(const HttpResponsePtr &)> &&callback);
};