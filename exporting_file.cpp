#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// Function to parse multipart/form-data
std::string parseMultipartFormData(const std::string& body, const std::string& boundary, const std::string& filename) {
    size_t start = body.find("filename=\"" + filename + "\"");
    if (start == std::string::npos) {
        throw std::runtime_error("File not found in the provided data");
    }

    // Find the start of the file content
    start = body.find("\r\n\r\n", start);
    if (start == std::string::npos) {
        throw std::runtime_error("Invalid multipart/form-data format");
    }
    start += 4; // Skip past "\r\n\r\n"

    // Find the end of the file content
    size_t end = body.find(boundary, start);
    if (end == std::string::npos) {
        throw std::runtime_error("Boundary not found after file content");
    }
    end -= 4; // Skip back past the "\r\n--"

    return body.substr(start, end - start);
}

// Function to save a binary string to a file
void saveToFile(const std::string& data, const std::string& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file for writing");
    }
    file.write(data.data(), data.size());
    file.close();
}

int main() {
    // Sample multipart/form-data body and boundary
    std::string body = "----------------------------722429892666999078490056\r\n"
                       "Content-Disposition: form-data; name=\"test\"; filename=\"Photo on 5-1-24 at 3.54 PM.jpg\"\r\n"
                       "Content-Type: image/jpeg\r\n\r\n"
                       "�����\\�F&]�� �k����Dv�����5�*����I��m�z����3�I������ʰ|o#.�e��-up��e����XV�B    <qN��|�H<0�3ӵ}��Y��ls���XW���\r\n"
                       "----------------------------722429892666999078490056--";
    std::string boundary = "----------------------------722429892666999078490056";

    try {
        std::string filename = "Photo on 5-1-24 at 3.54 PM.jpg";
        std::string fileContent = parseMultipartFormData(body, boundary, filename);
        saveToFile(fileContent, "output.jpg");
        std::cout << "File saved successfully" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
