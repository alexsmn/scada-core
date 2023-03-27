#pragma once

#include <base/bind.h>
#include <base/task_runner.h>
#include <memory>

template <class Lambda, class... Args>
inline void BindLambdaHelper(const std::shared_ptr<Lambda>& wrapped_task,
                             const Args&... args) {
  (*wrapped_task)(args...);
}

template <class Lambda>
inline auto BindLambda(Lambda&& lambda) {
  auto wrapped_task = std::make_shared<Lambda>(std::forward<Lambda>(lambda));
  return base::BindOnce(&BindLambdaHelper<Lambda>, wrapped_task);
}

template <class Lambda>
class WrappedLambda {
 public:
  template <class T>
  WrappedLambda(scoped_refptr<base::TaskRunner> task_runner,
                const base::Location& location,
                T&& lambda)
      : task_runner_{std::move(task_runner)},
        location_{location},
        wrapped_task_{std::make_shared<Lambda>(std::forward<T>(lambda))} {}

  template <class... Args>
  void operator()(const Args&... args) const {
    /*task_runner_->PostTask(location_,
       base::BindOnce(&BindLambdaHelper<Lambda>, wrapped_task_, args...));*/
    task_runner_->PostTask(location_,
                           BindLambda([=, wrapped_task = wrapped_task_] {
                             (*wrapped_task)(args...);
                           }));
  }

 private:
  scoped_refptr<base::TaskRunner> task_runner_;
  base::Location location_;
  std::shared_ptr<Lambda> wrapped_task_;
};

template <class Lambda>
inline auto WrapTaskRunner(scoped_refptr<base::TaskRunner> task_runner,
                           const base::Location& location,
                           Lambda&& lambda) {
  return WrappedLambda<Lambda>{std::move(task_runner), location,
                               std::forward<Lambda>(lambda)};
}
