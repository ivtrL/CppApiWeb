#include "Profile.h"

namespace Model
{
    Profile::Profile(const drogon::orm::DbClientPtr &app)
    {
        this->app = app;
    };

    Profile::Profile(const drogon::orm::DbClientPtr &app, std::string email)
    {
        this->app = app;
        this->email = email;
    }

    CrudResponse Profile::ExistsOnDb()
    {
        if (this->email.empty())
        {
            return CrudResponse{false, "Email is missing"};
        }

        std::future<drogon::orm::Result> future = this->app->execSqlAsyncFuture("select * from \"Profile\" where (email = $1)", email);
        try
        {
            drogon::orm::Result result = future.get();
            for (drogon::orm::Row row : result)
            {
                if (row["email"].as<std::string>() == email)
                {
                    return CrudResponse{true, "User exists"};
                }
            }
        }

        catch (const drogon::orm::DrogonDbException &e)
        {
            return CrudResponse{false, "Internal server error"};
        }

        return CrudResponse{false, "User does not exist"};
    }

    CrudResponse Profile::GetFromDb()
    {
        if (!ExistsOnDb().success)
        {
            return CrudResponse{false, "User does not exist"};
        }

        std::future<drogon::orm::Result> future = this->app->execSqlAsyncFuture("select * from \"Profile\" where (email = $1)", email);
        try
        {
            drogon::orm::Result result = future.get();
            for (drogon::orm::Row row : result)
            {
                this->id = row["id"].as<int>();
                this->name = row["name"].as<std::string>();
                this->imageUrl = row["imageUrl"].as<std::string>();
                this->password = row["password"].as<std::string>();
                this->updatedAt = row["updatedAt"].as<time_t>();
                this->createdAt = row["createdAt"].as<time_t>();
            }
            return CrudResponse{true, "User retrieved"};
        }

        catch (const drogon::orm::DrogonDbException &e)
        {
            return CrudResponse{false, "Internal server error"};
        }
    }

    CrudResponse Profile::InsertInDb()
    {
        if (ExistsOnDb().success)
        {
            return CrudResponse{false, "User already exists"};
        }
        std::future<drogon::orm::Result> future = this->app->execSqlAsyncFuture("insert into \"Profile\" (\"imageUrl\", name, email, password, \"updatedAt\") values ($1, $2, $3, $4, $5)",
                                                                                this->imageUrl,
                                                                                this->name,
                                                                                this->email,
                                                                                this->password,
                                                                                this->updatedAt);

        try
        {
            drogon::orm::Result result = future.get();
            return CrudResponse{true, "User created"};
        }

        catch (const drogon::orm::DrogonDbException &e)
        {
            return CrudResponse{false, "Internal server error"};
        }
    }

    CrudResponse Profile::DeleteFromDb()
    {
        if (!ExistsOnDb().success)
        {
            return CrudResponse{false, "User does not exist"};
        }

        std::future<drogon::orm::Result> future = this->app->execSqlAsyncFuture("delete from \"Profile\" where (email = $1)", email);
        try
        {
            drogon::orm::Result result = future.get();
            return CrudResponse{true, "User deleted"};
        }

        catch (const drogon::orm::DrogonDbException &e)
        {
            return CrudResponse{false, "Internal server error"};
        }
    }

    CrudResponse Profile::UpdateInDb()
    {
        if (!ExistsOnDb().success)
        {
            return CrudResponse{false, "User does not exist"};
        }

        std::future<drogon::orm::Result> future = this->app->execSqlAsyncFuture("update \"Profile\" set \"imageUrl\" = $1, name = $2, password = $3, \"updatedAt\" = $4 where (email = $5)",
                                                                                this->imageUrl,
                                                                                this->name,
                                                                                this->password,
                                                                                this->updatedAt,
                                                                                this->email);
        try
        {
            drogon::orm::Result result = future.get();
            return CrudResponse{true, "User updated"};
        }

        catch (const drogon::orm::DrogonDbException &e)
        {
            return CrudResponse{false, "Internal server error"};
        }
    }

    int Profile::getId() const
    {
        return id;
    }

    std::string Profile::getEmail() const
    {
        return email;
    }

    void Profile::setEmail(std::string email)
    {
        setUpdatedAt(time(nullptr));
        this->email = email;
    }

    std::string Profile::getPassword() const
    {
        return password;
    }

    void Profile::setPassword(std::string password)
    {
        setUpdatedAt(time(nullptr));
        this->password = password;
    }

    std::string Profile::getName() const
    {
        return name;
    }

    void Profile::setName(std::string name)
    {
        setUpdatedAt(time(nullptr));
        this->name = name;
    }

    std::string Profile::getImageUrl() const
    {
        return imageUrl;
    }

    void Profile::setImageUrl(std::string imageUrl)
    {
        setUpdatedAt(time(nullptr));
        this->imageUrl = imageUrl;
    }

    time_t Profile::getUpdatedAt() const
    {
        return updatedAt;
    }

    void Profile::setUpdatedAt(time_t updatedAt)
    {
        this->updatedAt = updatedAt;
    }

    time_t Profile::getCreatedAt() const
    {
        return createdAt;
    }
}