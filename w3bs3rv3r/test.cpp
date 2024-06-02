Certainly! I'll integrate these additional attributes (`servSockets`, `serverName`, `host`, `ports`) into the `Server` class and update the parsing logic accordingly.

### Step 1: Include Necessary Headers

```cpp
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
```

### Step 2: Define the Location Class

```cpp
class Location {
public:
    std::string path;
    std::map<std::string, std::string> settings;

    Location(const std::string& path = "") : path(path) {}

    void addSetting(const std::string& key, const std::string& value) {
        settings[key] = value;
    }

    void print() const {
        std::cout << "  Location (" << path << "):\n";
        for (std::map<std::string, std::string>::const_iterator it = settings.begin(); it != settings.end(); ++it) {
            std::cout << "    " << it->first << " = " << it->second << "\n";
        }
    }
};
```

### Step 3: Define the Server Class

```cpp
class Server {
public:
    std::vector<int> servSockets;
    std::string serverName;
    std::string host;
    std::vector<int> ports;
    std::map<std::string, std::string> settings;
    std::vector<Location> locations;

    void addSetting(const std::string& key, const std::string& value) {
        settings[key] = value;
    }

    void addLocation(const Location& location) {
        locations.push_back(location);
    }

    void print() const {
        std::cout << "Server Name: " << serverName << "\n";
        std::cout << "Host: " << host << "\n";
        std::cout << "Ports: ";
        for (size_t i = 0; i < ports.size(); ++i) {
            std::cout << ports[i];
            if (i < ports.size() - 1) std::cout << ", ";
        }
        std::cout << "\n";
        
        std::cout << "Server settings:\n";
        for (std::map<std::string, std::string>::const_iterator it = settings.begin(); it != settings.end(); ++it) {
            std::cout << it->first << " = " << it->second << "\n";
        }

        std::cout << "Locations:\n";
        for (size_t i = 0; i < locations.size(); ++i) {
            locations[i].print();
        }
    }
};
```

### Step 4: Define the ConfigParser Class

```cpp
class ConfigParser {
public:
    std::vector<Server> parseFile(const std::string& filename) {
        std::ifstream file(filename.c_str());
        servers.clear();
        inServerBlock = false;
        inLocationBlock = false;

        if (!file.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return servers;
        }

        std::string line;
        currentServer = Server();
        currentLocation = Location();

        while (std::getline(file, line)) {
            line = trim(line);

            if (line.empty() || line[0] == '#') continue;

            if (line == "server {") {
                if (inServerBlock) {
                    servers.push_back(currentServer);
                    currentServer = Server();
                }
                inServerBlock = true;
                continue;
            }

            if (line == "}") {
                if (inLocationBlock) {
                    currentServer.addLocation(currentLocation);
                    currentLocation = Location();
                    inLocationBlock = false;
                } else if (inServerBlock) {
                    servers.push_back(currentServer);
                    currentServer = Server();
                    inServerBlock = false;
                }
                continue;
            }

            if (line.find("location") == 0 && line.find("[") != std::string::npos) {
                if (inLocationBlock) {
                    currentServer.addLocation(currentLocation);
                    currentLocation = Location();
                }
                inLocationBlock = true;
                size_t start = line.find("(") + 1;
                size_t end = line.find(")");
                currentLocation.path = trim(line.substr(start, end - start));
                continue;
            }

            if (inLocationBlock && line.find("]") != std::string::npos) {
                currentServer.addLocation(currentLocation);
                currentLocation = Location();
                inLocationBlock = false;
                continue;
            }

            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = trim(line.substr(0, delimiterPos));
                std::string value = trim(line.substr(delimiterPos + 1));

                if (inLocationBlock) {
                    currentLocation.addSetting(key, value);
                } else if (inServerBlock) {
                    if (key == "serverName") {
                        currentServer.serverName = value;
                    } else if (key == "host") {
                        currentServer.host = value;
                    } else if (key == "port") {
                        std::stringstream ss(value);
                        std::string port;
                        while (std::getline(ss, port, ',')) {
                            currentServer.ports.push_back(std::stoi(port));
                        }
                    } else {
                        currentServer.addSetting(key, value);
                    }
                }
            }
        }

        if (inServerBlock) {
            servers.push_back(currentServer);
        }

        file.close();
        return servers;
    }

private:
    std::vector<Server> servers;
    Server currentServer;
    Location currentLocation;
    bool inServerBlock;
    bool inLocationBlock;

    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, last - first + 1);
    }
};
```

### Step 5: Main Function

```cpp
int main() {
    std::string filename = "config.conf";
    ConfigParser parser;
    std::vector<Server> servers = parser.parseFile(filename);

    for (size_t i = 0; i < servers.size(); ++i) {
        std::cout << "Server " << i + 1 << ":\n";
        servers[i].print();
        std::cout << std::endl;
    }

    return 0;
}
```

### Explanation

- **Location Class**: Stores path and settings of a location block. It provides methods to add a setting and print the location details.
- **Server Class**: Now includes `servSockets`, `serverName`, `host`, and `ports` in addition to settings and locations. Methods are provided to add a setting, add a location, and print server details.
- **ConfigParser Class**: Handles parsing the configuration file. It includes member variables for storing the current state of parsing (`servers`, `currentServer`, `currentLocation`, `inServerBlock`, `inLocationBlock`). The `parseFile` method reads and processes the file, updating the state as it encounters different blocks and settings. A helper method `trim` is used to remove leading and trailing whitespace from strings.
- **Main Function**: Demonstrates how to use the `ConfigParser` class to parse a configuration file and print the parsed servers and their locations.

This version integrates the additional attributes into the `Server` class and updates the parsing logic to handle these attributes correctly.
