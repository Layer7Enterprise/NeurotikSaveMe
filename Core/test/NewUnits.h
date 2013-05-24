#ifndef NEW_UNITS_H_
#define NEW_UNITS_H_

#include <CCup.h>

void RunUnits() {
  CCup(function() {
      Describe("test", function() {
        It("hey", function() {
          IsEqual(1, 1);
        });

        SetTimeout(2);
        It("can also", _function() {
          IsEqual(1, 1);
          sleep(1);
          done();
        });
      });
  });

  while (1);
}

#endif
