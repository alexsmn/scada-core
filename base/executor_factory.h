#pragma once

#include <functional>
#include <memory>

class Executor;

using ExecutorFactory = std::function<std::shared_ptr<Executor>()>;
