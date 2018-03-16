//
// Created by George on 3/16/18.
//

#ifndef MILES_WAL_H
#define MILES_WAL_H


#include <fstream>
#include <string>
#include "results.h"

class Wal {

public:

    explicit Wal(std::string path):
            file(path, std::fstream::in | std::fstream::out | std::fstream::trunc) {;}

    bool log(std::string key, const json* payload);

    bool clear();


private:

    std::fstream file;

};


#endif //MILES_WAL_H
