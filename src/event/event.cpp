#include "Arduino.h"
#include "event.h"
#include "../datetime/datetime.h" // Include DateTime custom class

Event::Event(String subjectName, DateTime startDateTimeValue, DateTime endDateTimeValue) : subject(subjectName), startDateTime(startDateTimeValue), endDateTime(endDateTimeValue) {}
