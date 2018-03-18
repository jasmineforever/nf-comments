#include <iostream>
#include "Service.h"
using namespace std;
using namespace nifeng;

int main(int argc, char* argv[])
{
    std::cout<<"Using -c config_file to run with special config.\nIf not, default config:./nf_config.json will be used. "<<std::endl;
    std::string config_file = "./nf_config.json";
    if(argc == 3)
    {
        if(strcmp("-c", argv[1]) == 0)
        {
            config_file = argv[2];
        }
    }
    Service service;
    LOG_TRACE("using config:%s", config_file.c_str());
    if(!Config::loadFromFile(config_file))
    {
        exit(EXIT_FAILURE);
    }
    if(!service.run())
    {
        exit(EXIT_FAILURE);
    }
    service.join();
    return 0;
}