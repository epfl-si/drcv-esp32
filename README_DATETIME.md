# DateTime Class

## Requirements

This class required the `Split()` function of this project

## SYNTHAX

```arduino
DateTime* <varname> = new DateTime(<ISO 8601 datetime string>);
```

## USAGES

Copy `datetime.cpp` and `datetime.h` at the root of your project.

```arduino
#include "datetime.h"

DateTime* current_date = new DateTime("2025-11-01T12:56:31");

Serial.println(current_date->year);
Serial.println(current_date->month);
Serial.println(current_date->day);
Serial.println();
Serial.println(current_date->hour);
Serial.println(current_date->minute);
Serial.println(current_date->second);



>>> "2025"
>>> "11"
>>> "01"
>>>
>>> "12"
>>> "56"
>>> "31"
```