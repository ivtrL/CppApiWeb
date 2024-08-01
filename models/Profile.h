#pragma once
#include <drogon/drogon.h>

namespace Model
{
    struct CrudResponse
    {
        bool success;
        std::string message;
    };

    class Profile
    {
    public:
        Profile(const drogon::orm::DbClientPtr &app);
        Profile(const drogon::orm::DbClientPtr &app, std::string email);

        CrudResponse ExistsOnDb();
        CrudResponse GetFromDb();
        CrudResponse InsertInDb();
        CrudResponse DeleteFromDb();
        CrudResponse UpdateInDb();

        int getId() const;
        std::string getEmail() const;
        void setEmail(std::string email);
        std::string getPassword() const;
        void setPassword(std::string password);
        std::string getName() const;
        void setName(std::string name);
        std::string getImageUrl() const;
        void setImageUrl(std::string imageUrl);
        time_t getUpdatedAt() const;
        void setUpdatedAt(time_t updatedAt);
        time_t getCreatedAt() const;

    private:
        drogon::orm::DbClientPtr app;
        int id;
        std::string email;
        std::string password;
        std::string name;
        std::string imageUrl;
        time_t updatedAt;
        time_t createdAt;
    };
}