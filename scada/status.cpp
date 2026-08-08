#include "scada/status.h"

#if defined(SCADA_USE_BASE_MODULE)
// Modules-pilot consumer (SCADA_CXX_MODULES=ON): base names come from the
// scada.base facade. The import sits after the textual includes because the
// reverse order trips an AppleClang 21 declaration-merging bug in libc++.
import scada.base;
#else
#include "base/ui_text.h"
#endif

namespace scada {

// static
Status Status::FromFullCode(unsigned full_code) {
  Status result(StatusCode::Bad);
  result.full_code_ = full_code;
  return result;
}

}  // namespace scada

namespace {

struct Entry {
  scada::StatusCode code;
  // The enum spelling, for logs and wire diagnostics. Never translated.
  const char* error_string;
  // The operator-facing sentence, in English. `ToString16` runs it through
  // `TranslateUiText`, so the Russian lives in the client's `client_ru.ts`
  // catalog keyed by this exact string — change one and you must change the
  // other, or the client silently falls back to English.
  const char* display_string;
};

const Entry kEntries[] = {
    {scada::StatusCode::Good, "Good", "Operation completed successfully"},
    {scada::StatusCode::Good_Pending, "Good_Pending", "Operation in progress"},
    {scada::StatusCode::Uncertain_StateWasNotChanged,
     "Uncertain_StateWasNotChanged", "The lock was not changed"},
    {scada::StatusCode::Bad, "Bad", "Error"},
    {scada::StatusCode::Bad_WrongLoginCredentials, "Bad_WrongLoginCredentials",
     "Wrong user name or password"},
    {scada::StatusCode::Bad_UserIsAlreadyLoggedOn, "Bad_UserIsAlreadyLoggedOn",
     "A session for this user is already open"},
    {scada::StatusCode::Bad_UnsupportedProtocolVersion,
     "Bad_UnsupportedProtocolVersion", "Protocol version is not supported"},
    {scada::StatusCode::Bad_ObjectIsBusy, "Bad_ObjectIsBusy",
     "Another command is already running"},
    {scada::StatusCode::Bad_WrongNodeId, "Bad_WrongNodeId",
     "Wrong node identifier"},
    {scada::StatusCode::Bad_WrongDeviceId, "Bad_WrongDeviceId",
     "Wrong device identifier"},
    {scada::StatusCode::Bad_Disconnected, "Bad_Disconnected", "Not connected"},
    {scada::StatusCode::Bad_SessionForcedLogoff, "Bad_SessionForcedLogoff",
     "Session closed because this user connected again"},
    {scada::StatusCode::Bad_Timeout, "Bad_Timeout",
     "Operation aborted after the wait timed out"},
    {scada::StatusCode::Bad_CantDeleteDependentNode,
     "Bad_CantDeleteDependentNode",
     "Cannot delete the object because dependent objects exist"},
    {scada::StatusCode::Bad_ServerWasShutDown, "Bad_ServerWasShutDown",
     "Session closed because the server stopped"},
    {scada::StatusCode::Bad_WrongMethodId, "Bad_WrongMethodId",
     "The command is not supported by this object"},
    {scada::StatusCode::Bad_CantDeleteOwnUser, "Bad_CantDeleteOwnUser",
     "Cannot delete a user from a session that user opened"},
    {scada::StatusCode::Bad_DuplicateNodeId, "Bad_DuplicateNodeId",
     "An object with this identifier already exists"},
    {scada::StatusCode::Bad_UnsupportedFileVersion,
     "Bad_UnsupportedFileVersion", "File version is not supported"},
    {scada::StatusCode::Bad_WrongTypeId, "Bad_WrongTypeId",
     "Wrong object type"},
    {scada::StatusCode::Bad_WrongParentId, "Bad_WrongParentId",
     "Wrong parent object identifier"},
    {scada::StatusCode::Bad_SessionIsLoggedOff, "Bad_SessionIsLoggedOff",
     "Not logged on"},
    {scada::StatusCode::Bad_WrongSubscriptionId, "Bad_WrongSubscriptionId",
     "Wrong subscription number"},
    {scada::StatusCode::Bad_WrongIndex, "Bad_WrongIndex", "Wrong index"},
    {scada::StatusCode::Bad_Iec60870UnknownType, "Bad_IecUnknownType",
     "Wrong IEC 60870-5 ASDU type"},
    {scada::StatusCode::Bad_Iec60870UnknownCot, "Bad_IecUnknownCot",
     "Wrong IEC 60870-5 cause of transmission"},
    {scada::StatusCode::Bad_Iec60870UnknownDevice, "Bad_IecUnknownDevice",
     "Wrong IEC 60870-5 device address"},
    {scada::StatusCode::Bad_Iec60870UnknownAddress, "Bad_IecUnknownAddress",
     "Wrong IEC 60870-5 information object address"},
    {scada::StatusCode::Bad_Iec60870UnknownError, "Bad_IecUnknownError",
     "IEC 60870-5 protocol error"},
    {scada::StatusCode::Bad_WrongCallArguments, "Bad_WrongCallArguments",
     "Wrong command arguments"},
    {scada::StatusCode::Bad_CantParseString, "Bad_CantParseString",
     "Cannot convert the string to a value of this type"},
    {scada::StatusCode::Bad_TooLongString, "Bad_TooLongString",
     "String is too long"},
    {scada::StatusCode::Bad_WrongPropertyId, "Bad_WrongPropertyId",
     "Wrong object attribute"},
    {scada::StatusCode::Bad_WrongReferenceId, "Bad_WrongReferenceId",
     "Wrong reference type"},
    {scada::StatusCode::Bad_WrongNodeClass, "Bad_WrongNodeClass",
     "Wrong node class"},
    {scada::StatusCode::Bad_Iec61850Error, "Bad_Iec61850Error",
     "IEC 61850 protocol error"},
    {scada::StatusCode::Bad_NothingToDo, "Bad_NothingToDo",
     "The request is empty"},
    {scada::StatusCode::Bad_BrowseNameInvalid, "Bad_BrowseNameInvalid",
     "Name not found"},
    {scada::StatusCode::Bad_MonitoredItemIdInvalid,
     "Bad_MonitoredItemIdInvalid", "Wrong monitored item number"},
    {scada::StatusCode::Bad_MessageNotAvailable, "Bad_MessageNotAvailable",
     "The requested message is no longer available"},
    {scada::StatusCode::Bad_ApplicationSignatureInvalid,
     "Bad_ApplicationSignatureInvalid", "Invalid client application signature"},
    {scada::StatusCode::Bad_TooManyOperations, "Bad_TooManyOperations",
     "Too many operations in the request"},
    {scada::StatusCode::Bad_TooManyMonitoredItems, "Bad_TooManyMonitoredItems",
     "Too many monitored items in the request"},
    {scada::StatusCode::Bad_SequenceNumberUnknown, "Bad_SequenceNumberUnknown",
     "Unknown message sequence number"},
    {scada::StatusCode::Bad_NoContinuationPoints, "Bad_NoContinuationPoints",
     "The browse continuation point limit is exhausted"},
    {scada::StatusCode::Bad_TimestampsToReturnInvalid,
     "Bad_TimestampsToReturnInvalid", "Wrong TimestampsToReturn value"},
    {scada::StatusCode::Bad_ViewIdUnknown, "Bad_ViewIdUnknown",
     "Unknown view identifier"},
    {scada::StatusCode::Bad_HistoryOperationInvalid,
     "Bad_HistoryOperationInvalid", "Invalid history request parameters"},
    {scada::StatusCode::Bad_NoSubscription, "Bad_NoSubscription",
     "The session has no subscriptions"},
    {scada::StatusCode::Bad_UserAccessDenied, "Bad_UserAccessDenied",
     "Not enough rights to perform the operation"},
    {scada::StatusCode::Bad_NotSupported, "Bad_NotSupported",
     "Operation is not supported"},
    {scada::StatusCode::Bad_LicenseExpired, "Bad_LicenseExpired",
     "The license has expired"},
    {scada::StatusCode::Bad_WaitingForInitialData, "Bad_WaitingForInitialData",
     "No value received from the data source yet"},
    {scada::StatusCode::Bad_OutOfRange, "Bad_OutOfRange",
     "The value is out of range and will not be stored"},
};

const Entry* FindEntry(scada::StatusCode status_code) {
  for (auto& entry : kEntries) {
    if (entry.code == status_code)
      return &entry;
  }
  return nullptr;
}

}  // namespace

const char* ToCString(scada::StatusCode status_code) {
  if (auto* entry = FindEntry(status_code))
    return entry->error_string;

  return IsGood(status_code) ? "OK" : "Error";
}

std::string ToString(scada::StatusCode status_code) {
  return std::string{ToCString(status_code)};
}

std::u16string ToString16(scada::StatusCode status_code) {
  if (auto* entry = FindEntry(status_code))
    return scada::TranslateUiText(entry->display_string);

  return scada::TranslateUiText(
      IsGood(status_code) ? "Operation completed successfully" : "Error");
}

std::string ToString(const scada::Status& status) {
  return ToString(status.code());
}

std::u16string ToString16(const scada::Status& status) {
  return ToString16(status.code());
}
