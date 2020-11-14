#pragma once

#include "core/data_services.h"
#include "core/logging.h"

#include <functional>
#include <memory>
#include <string_view>
#include <vector>

namespace boost::asio {
class io_context;
}

namespace net {
class TransportFactory;
}

class Logger;

struct DataServicesContext {
  const std::shared_ptr<Logger> logger;
  boost::asio::io_context& io_context;
  net::TransportFactory& transport_factory;
  scada::ServiceLogParams service_log_params;
};

using DataServicesFactoryMethod =
    std::function<bool(const DataServicesContext& context,
                       DataServices& services)>;

struct DataServicesInfo {
  std::string name;
  std::wstring display_name;
  DataServicesFactoryMethod factory_method;
};

void RegisterDataServices(DataServicesInfo info);

using DataServicesInfoList = std::vector<DataServicesInfo>;

const DataServicesInfoList& GetDataServicesInfoList();

bool EqualDataServicesName(std::string_view name1, std::string_view name2);

#define REGISTER_DATA_SERVICES(name, display_name, factory_method) \
  static bool factory_method##_registered = [] {                   \
    RegisterDataServices({name, display_name, factory_method});    \
    return true;                                                   \
  }();

bool CreateDataServices(std::string_view name,
                        const DataServicesContext& context,
                        DataServices& services);
