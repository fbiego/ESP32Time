/*
   MIT License

  Copyright (c) 2021 Felix Biego

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#ifndef ESP32TIME_H
#define ESP32TIME_H

#include <Arduino.h>

class ESP32Time {
	
	public:
		ESP32Time();
		ESP32Time(long offset);
		void setTime(unsigned long epoch = 1609459200, int ms = 0) const;	// default (1609459200) = 1st Jan 2021
		void setTime(int sc, int mn, int hr, int dy, int mt, int yr, int ms = 0) const;
		void setTimeStruct(tm t) const;
		tm getTimeStruct() const;
		String getTime(String format) const;
		
		String getTime() const;
		String getDateTime(bool mode = false) const;
		String getTimeDate(bool mode = false) const;
		String getDate(bool mode = false) const;
		String getAmPm(bool lowercase = false) const;
		
		unsigned long getEpoch() const;
		unsigned long getMillis() const;
		unsigned long getMicros() const;
		int getSecond() const;
		int getMinute() const;
		int getHour(bool mode = false) const;
		int getDay() const;
		int getDayofWeek() const;
		int getDayofYear() const;
		int getMonth() const;
		int getYear() const;
		
		long offset = 0;
		unsigned long getLocalEpoch() const;
		
		

};


#endif
