#include <Arduino.h>

#ifndef XML_PARSER_H
#define XML_PARSER_H

String* XMLParser(String param, const char* startParse, const char* endParse, int &len);
String XMLGetter(String param, const char* startParse, const char* endParse);

#endif
