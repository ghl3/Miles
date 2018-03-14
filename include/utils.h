//
// Created by George on 3/13/18.
//

#ifndef MILES_UTILS_H
#define MILES_UTILS_H

#include <string>
#include <utility>
#include <sys/stat.h>
#include <sstream>
#include <iostream>

#include <boost/filesystem.hpp>


std::string generateRandomId(size_t length = 0);

class TempDirectory {

public:

    explicit TempDirectory(const std::string &prefix): path((std::stringstream() << prefix << generateRandomId(12)).str()) {
        boost::filesystem::create_directories(path.c_str());
    }

    ~TempDirectory() {
        boost::filesystem::remove_all(this->path.c_str());
    }

    const std::string& getPath() const {
        return this->path;
    }

private:
    const std::string path;
};


#endif //MILES_UTILS_H
