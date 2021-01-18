#include "file_utils.hpp"

bool readFile(fs::FS &fs, const char * path,String& readBuffer){
    // TODO make this like here https://github.com/adjavaherian/solar-server/blob/master/lib/Poster/Poster.cpp
    Serial.printf("Reading file: %s\r\n", path);

    readBuffer = "";
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return false;
    }

    Serial.println("- read from file:");
    while(file.available()){
        String curr = file.readString();
        readBuffer.concat(curr);
    }
    file.close();
    return true;
}