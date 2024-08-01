#include <drogon/drogon.h>
#include "include/dotenv.h"

int main()
{
    dotenv::init();
    drogon::app().loadConfigFile("../config.yaml");
    drogon::app().run();
    return 0;
}
