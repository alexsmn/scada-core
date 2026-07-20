#include "base/observer_list.h"

#include <gtest/gtest.h>

namespace {

struct TestObserver {
  int calls = 0;
};

}  // namespace

TEST(ObserverListTest, RemoveAfterRangeIterationErasesObserver) {
  scada::base::ObserverList<TestObserver> observers;
  TestObserver observer;
  observers.AddObserver(&observer);

  for (auto& item : observers) {
    ++item.calls;
  }

  observers.RemoveObserver(&observer);

  EXPECT_EQ(observer.calls, 1);
  EXPECT_FALSE(observers.might_have_observers());
}

TEST(ObserverListTest, RemoveDuringRangeIterationCompactsAfterLoop) {
  scada::base::ObserverList<TestObserver> observers;
  TestObserver first;
  TestObserver second;
  observers.AddObserver(&first);
  observers.AddObserver(&second);

  for (auto& item : observers) {
    ++item.calls;
    if (&item == &first) {
      observers.RemoveObserver(&first);
    }
  }

  EXPECT_FALSE(observers.HasObserver(&first));
  EXPECT_TRUE(observers.HasObserver(&second));

  observers.RemoveObserver(&second);

  EXPECT_EQ(first.calls, 1);
  EXPECT_EQ(second.calls, 1);
  EXPECT_FALSE(observers.might_have_observers());
}

