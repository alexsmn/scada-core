#include "data_services_factory.h"

#include "base/string_piece_util.h"
#include "base/strings/string_util.h"

namespace {

DataServicesInfoList& GetMutableDataServicesInfoList() {
  static DataServicesInfoList s_list;
  return s_list;
}

}  // namespace

const DataServicesInfoList& GetDataServicesInfoList() {
  return GetMutableDataServicesInfoList();
}

void RegisterDataServices(DataServicesInfo info) {
  GetMutableDataServicesInfoList().emplace_back(std::move(info));
}

bool EqualDataServicesName(std::string_view name1, std::string_view name2) {
  return base::EqualsCaseInsensitiveASCII(ToStringPiece(name1),
                                          ToStringPiece(name2));
}

bool CreateDataServices(std::string_view name,
                        const DataServicesContext& context,
                        DataServices& services) {
  for (auto& info : GetDataServicesInfoList()) {
    if (EqualDataServicesName(info.name, name))
      return info.factory_method(context, services);
  }
  return false;
}
