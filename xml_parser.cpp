#include "xml_parser.h"
#include <Arduino.h>

int GetStringOccurence(String text, String occurenceText){
  int count = 0;
  int index = 0;
  while ((index = text.indexOf(occurenceText, index)) != -1) {
      count++;
      index += occurenceText.length(); // Move past last found occurrence
  }
  return count;
}

String* XMLParser(String param, const char* startParse, const char* endParse, int &len) {  
  int array_size = GetStringOccurence(param, startParse);
  String* AllOccurences = new String[array_size];
  int index = 0;
  int indexString = 0;
  while ((indexString = param.indexOf(startParse, indexString)) != -1) {
      AllOccurences[index] = param.substring(param.indexOf(startParse, indexString),param.indexOf(endParse, indexString) + strlen(endParse));
      index++;
      indexString += strlen(startParse);
  }
  len = array_size;
  return AllOccurences;
}

String XMLGetter(String param, const char* startParse, const char* endParse) {
  return param.substring(param.indexOf(startParse) + strlen(startParse),param.indexOf(endParse));
}
