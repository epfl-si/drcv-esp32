#include <Arduino.h>
#include "datetime.h" // Include DateTime custom class

#ifndef EVENT_H
#define EVENT_H

class Event {
public:
  String subject;
  DateTime startDateTime;
  DateTime endDateTime;
  Event(String subjectName, DateTime startDateTimeValue, DateTime endDateTimeValue);
};

#endif
