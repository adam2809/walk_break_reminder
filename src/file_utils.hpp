#include <SPIFFS.h>

bool readFile(fs::FS &fs, const char * path,String& readBuffer);
void writeFile(fs::FS &fs, const char * path, const char * message);
void listDir(fs::FS &fs, const char * dirname, uint8_t levels);



