# SPLIT() Function

## SYNTHAX

```arduino
String* <varname> = Split(<String text>, <String separator> <Int length_result>);
```

## USAGES

Copy `split.cpp` and `split.h` at the root of your project.

```arduino
#include "split.h"

int ArrayLength = 0;
int ArrayLength2 = 0;
String* Array = Split("a,b,c", ",", ArrayLength);
String* Array2 = Split("lb.?am.?san,?jm,", "?", ArrayLength2);

Serial.println(ArrayLength);
Serial.println(Array[0]);
Serial.println(Array[1]);
Serial.println(Array[2]);
Serial.println();
Serial.println(ArrayLength2);
Serial.println(Array2[0]);
Serial.println(Array2[1]);
Serial.println(Array2[2]);
Serial.println(Array2[3]);



>>> 3
>>> "a"
>>> "b"
>>> "c"
>>>
>>> 4
>>> "lb."
>>> "am."
>>> "san,"
>>> "jm,"
```