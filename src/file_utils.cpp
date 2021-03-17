#include "file_utils.hpp"

bool readFile(fs::FS &fs, const char * path,String& readBuffer){
    log_d("Reading file: %s", path);

    readBuffer = "";
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        log_d("Failed to open file for reading");
        return false;
    }

    log_d("Successful read from file:");
    while(file.available()){
        String curr = file.readString();
        readBuffer.concat(curr);
    }
    file.close();
    return true;
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    log_d("Writing file to: %s", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        log_d("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        log_d("Successful file write");
    } else {
        log_d("Falied file write");
    }
    file.close();
}

JsonObject& loadConfig(DynamicJsonBuffer& jsonBuffer){
	String readBuffer;
	readFile(SPIFFS, "/config.json",readBuffer);
	return parseJson(jsonBuffer,readBuffer);
}