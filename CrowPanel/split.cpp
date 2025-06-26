#include "split.h"
#include <Arduino.h>

int GetStringOccurenceBeforeSplit(String text, String occurenceText){
  int count = 0;
  int index = 0;
  while ((index = text.indexOf(occurenceText, index)) != -1) {
      count++;
      index += occurenceText.length(); // Move past last found occurrence
  }
  return count;
}

String* Split(String text, const char* separator, int &len) {
  int index = 0;
  int prevIndex = 0;
  int array_size = GetStringOccurenceBeforeSplit(text, separator) + 1;
  len = array_size;
  String* values = new String[array_size];
  int indexArray = 0;
  while ((index = text.indexOf(separator, index)) != -1) {
    values[indexArray] = text.substring(prevIndex, text.indexOf(separator, index));
    index += strlen(separator);
    prevIndex = index;
    indexArray++;
  }
  values[indexArray] = text.substring(prevIndex);
  return values;
}
