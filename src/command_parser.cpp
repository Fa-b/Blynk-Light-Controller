#include "command_parser.h"

#include "typedefs.h"

enum Commands { 
    ping,
    update,
    ver
};

CommandParser::CommandParser() {
    cmd_map = {
        {".ping", ping},
        {".update", update},
        {".version", ver}
    };
}

int CommandParser::findCommand(String cmd) {
    for (auto entry : cmd_map)
        if(entry.first.compareTo(cmd) == 0) return entry.second;
    return -1;
}

BLYNK_WRITE(DEBUG_BRIDGE) {
    String msg = param.asString();
    String response;
    unsigned int len = msg.length() + 1;
    char buf[len];
    msg.toCharArray(buf, len);
    buf[len - 1] = '\0';
    String msg_id = strtok(buf, " ");
    String cmd = strtok(NULL, " ");
    String arg = strtok(NULL, " ");
    std::vector<String> args;
    while(!arg.isEmpty()) {
        args.push_back(arg);
        arg = strtok(NULL, " ");
    }

    switch(CmdParser.findCommand(cmd)) {
        case ping:
            response = "pong";
            break;
        case update:

            break;
        case ver:
            if(args.size() > 0) {
                if(args[0].compareTo("get") == 0) {
                    response = String("v") + String(version);
                } else if(args[0].compareTo("list") == 0) {
                    response = String("version list not ready yet :-/");
                } else if(args[0].compareTo("install") == 0 && args.size() > 1) {
                    loadFirmware(args[1]);
                } else {
                    response = String("Error: Unknown subcommand \'" + args[0] + "\'!");
                }
            } else {
                response = String("Error: Not enough arguments!");
            }
            break;
        default:
            response = String("Error: Command \'" + cmd + "\' not found!");
            break;
    }

    bridge.printf("%s %s", msg_id.c_str(), response.c_str());
    bridge.flush();
}

CommandParser CmdParser;