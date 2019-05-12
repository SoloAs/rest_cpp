#pragma once

const int MAX_CONFIG_SIZE = 1000;

struct config
{
    std::string mode;
    std::string company;
    std::string ip;
    std::string dns;
    bool dhcp;
    bool igmp;
};
