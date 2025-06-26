#include "Arduino.h"
#include "datetime.h"
#include "../src/split/split.h" // Include my Split method

DateTime::DateTime(String datetimeString) {
  int datetimeArrayLength = 0;
  while ((datetimeString.indexOf("Z")) != -1) {
    datetimeString.remove(datetimeString.indexOf("Z"), 1);
  }
  String* datetimeArray = Split(datetimeString, "T", datetimeArrayLength);

  int dateArrayLength = 0;
  int timeArrayLength = 0;
  String* dateArray = Split(datetimeArray[0], "-", dateArrayLength);
  String* timeArray = Split(datetimeArray[1], ":", timeArrayLength);

  year = dateArray[0];
  month = dateArray[1];
  day = dateArray[2];

  hour = timeArray[0];
  minute = timeArray[1];
  second = timeArray[2];
}
