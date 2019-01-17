// ---------------------------------------------------------
// median filter (c) 2019 by RainerU
// https://github.com/RainerU/medianfilter
// This code is under the MIT License (see github repository)
// first published 25.03.2016 by me at
// http://www.rf1000.de/viewtopic.php?t=1119&start=20#p12788
// updates: license, comments and one variable name
// ---------------------------------------------------------

// Median filter for a serial data stream.
// Can run on Arduino or similar C-based environments.
//
// The median is the middle value in an ordered list. This means that
// no values are averaged or modified in any way but outliers are removed.
// Useful for a continuous data stream which contains only few but
// maybe high outliers.
//
// Stores a static array with the last 3/5/7/... values of the data stream.
// Data type of the values can be changed to any C type.
// Each call replaces the oldest value of the array by the new one.
// The median value, which is in the middle of the ordered array, is returned.
// Fast processing: Only one loop. Ring structure instead of shift register.
// Order of unchanged values is reused and updated incrementally.
// Initialization at the first call: The array is filled with the first value.
// Sorry, only one channel. Copy complete function with renamed constants for more.
// (This allows to use different data types and compiles into faster code than
// a generic library.)

#define storageSize 7  // size of filter, only odd numbers 3/5/7/... allowed
#define medianPos  3   // position of median in ordered list: (storageSize-1)/2,
                       // don't let the processor calculate this in the loop
#define medianType int // type of data values (int/long/float/...)

medianType median(medianType newVal) {
   static int init = 1; // init flag
   static medianType storageVal[storageSize]; // array of values
   static medianType storageOrd[storageSize]; // array of order numbers
   static int current = 0; // current index of the ring storage
   medianType returnVal = newVal; // the new value could be the median

   if (init) {
      // initialization of arrays
      int i;
      for (i=0; i<storageSize; i++) {
         storageVal[i] = newVal; // fill array with new value
         storageOrd[i] = i;      // just spread all order values
      }
      init = 0; // don't run initialization again
   } else {
      // store new value
      if (++current >= storageSize) current=0; // go one step in ring storage
      medianType oldVal = storageVal[current]; // save old value
      int oldOrd = storageOrd[current]; // save old order number
      storageVal[current] = newVal; // store new value
      storageOrd[current] = 0; // reset order number for new value
   
      // main loop
      int c = current; // index for loop through ring storage
      if (++c >= storageSize) c=0; // go one step (don't process current position)
      do {
         if (newVal <= storageVal[c] && oldOrd > storageOrd[c])
            storageOrd[c]++; // bigger value removed, smaller value added
         else if (newVal > storageVal[c] && oldOrd < storageOrd[c])
            storageOrd[c]--; // smaller value removed, bigger value added
         if (storageOrd[c] == medianPos)
            returnVal = storageVal[c]; // median found
         if (newVal > storageVal[c])
            storageOrd[current]++; // calculate order of new value
         if (++c >= storageSize) c=0; // go one step in ring storage
      } while (c != current); // stop at current position
   }
   // return new median value
   return (returnVal);
}

// dummy arduino sketch, only to show how easy the filter is used:

#define analogPin A0
int analogVal;
int medianVal;

void setup()
{
}

void loop()
{
   // ...
   analogVal = analogRead(analogPin);
   medianVal = median(analogVal);
   // ...
}
