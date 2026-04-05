#pragma once

struct DataServicesContext;
struct DataServices;

bool CreateRemoteServices(const DataServicesContext& context,
                          DataServices& services);
