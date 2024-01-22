//
//    FILE: unit_test_001.cpp
//  AUTHOR: Trent M. Wyatt
//    DATE: 2024-01-04
// PURPOSE: unit tests for ArduinoCLI library
//          https://github.com/RobTillaart/SIMON
//


// supported assertions
// ----------------------------
// assertEqual(expected, actual);               // a == b
// assertNotEqual(unwanted, actual);            // a != b
// assertComparativeEquivalent(expected, actual);    // abs(a - b) == 0 or (!(a > b) && !(a < b))
// assertComparativeNotEquivalent(unwanted, actual); // abs(a - b) > 0  or ((a > b) || (a < b))
// assertLess(upperBound, actual);              // a < b
// assertMore(lowerBound, actual);              // a > b
// assertLessOrEqual(upperBound, actual);       // a <= b
// assertMoreOrEqual(lowerBound, actual);       // a >= b
// assertTrue(actual);
// assertFalse(actual);
// assertNull(actual);

// // special cases for floats
// assertEqualFloat(expected, actual, epsilon);    // fabs(a - b) <= epsilon
// assertNotEqualFloat(unwanted, actual, epsilon); // fabs(a - b) >= epsilon
// assertInfinity(actual);                         // isinf(a)
// assertNotInfinity(actual);                      // !isinf(a)
// assertNAN(arg);                                 // isnan(a)
// assertNotNAN(arg);                              // !isnan(a)


#include <ArduinoUnitTests.h>

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "Bang.h"

enum { RX_PIN = 7, TX_PIN = 8 };
SoftwareSerial command_serial(RX_PIN, TX_PIN);

// class wrapper for the Bang api so far using two serial ports:
Bang bang(command_serial, Serial);


unittest_setup()
{
  fprintf(stderr, "Bang Platform\n");
  Serial.begin(115200);
  command_serial.begin(38400);
}


unittest_teardown()
{
  Serial.end();
  command_serial.end();
}


unittest(test_mock)
{
}

unittest_main()

//  -- END OF FILE --


