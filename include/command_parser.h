/**
 * @file       command_parser.h
 * @author     Fa-b
 * @license    This project is released under the MIT License (MIT)
 * @date       Oct 2021
 * @brief      Parses Blynk '.' (dot) commands and distributes/responds
 */

#include <Arduino.h>
#include <map>
#include "OTA_updater.h"

#ifndef command_parser_h
#define command_parser_h

class CommandParser {
public:
    CommandParser();

    int findCommand(String);

private:
    std::map<String, int> cmd_map;
};

extern CommandParser CmdParser;

#endif /* command_parser_h */