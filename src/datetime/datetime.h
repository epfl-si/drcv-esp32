#ifndef DATETIME_H
#define DATETIME_H

class DateTime {
public:
  //Date (String to keep when value is under 10)
  String day;
  String month;
  String year;

  //Time (String to keep when value is under 10)
  String hour;
  String minute;
  String second;

  //Constructor
  DateTime(String datetimeString);
};

#endif
