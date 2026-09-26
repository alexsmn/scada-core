#include "scada/status.h"

#include <string_view>

namespace scada {

// static
Status Status::FromFullCode(unsigned full_code) {
  Status result(StatusCode::Bad);
  result.full_code_ = full_code;
  return result;
}

namespace {

// Set once at startup by the UI layer and read thereafter; a function-local
// static keeps it out of static-global-init ordering, as `SetUiTextTranslator`
// does.
StatusTextProvider& GetStatusTextProvider() {
  static StatusTextProvider provider = nullptr;
  return provider;
}

}  // namespace

void SetStatusTextProvider(StatusTextProvider provider) {
  GetStatusTextProvider() = provider;
}

}  // namespace scada

namespace {

struct Entry {
  scada::StatusCode code;
  // The enum spelling, for logs, wire diagnostics and — when no UI has
  // installed a text provider — for display. Never translated.
  const char* error_string = nullptr;
};

const Entry kEntries[] = {
    {scada::StatusCode::Good, "Good"},
    {scada::StatusCode::Good_Pending, "Good_Pending"},
    {scada::StatusCode::Uncertain_StateWasNotChanged,
     "Uncertain_StateWasNotChanged"},
    {scada::StatusCode::Bad, "Bad"},
    {scada::StatusCode::Bad_WrongLoginCredentials, "Bad_WrongLoginCredentials"},
    {scada::StatusCode::Bad_UserIsAlreadyLoggedOn, "Bad_UserIsAlreadyLoggedOn"},
    {scada::StatusCode::Bad_UnsupportedProtocolVersion,
     "Bad_UnsupportedProtocolVersion"},
    {scada::StatusCode::Bad_ObjectIsBusy, "Bad_ObjectIsBusy"},
    {scada::StatusCode::Bad_WrongNodeId, "Bad_WrongNodeId"},
    {scada::StatusCode::Bad_WrongDeviceId, "Bad_WrongDeviceId"},
    {scada::StatusCode::Bad_Disconnected, "Bad_Disconnected"},
    {scada::StatusCode::Bad_SessionForcedLogoff, "Bad_SessionForcedLogoff"},
    {scada::StatusCode::Bad_Timeout, "Bad_Timeout"},
    {scada::StatusCode::Bad_CantDeleteDependentNode,
     "Bad_CantDeleteDependentNode"},
    {scada::StatusCode::Bad_ServerWasShutDown, "Bad_ServerWasShutDown"},
    {scada::StatusCode::Bad_WrongMethodId, "Bad_WrongMethodId"},
    {scada::StatusCode::Bad_CantDeleteOwnUser, "Bad_CantDeleteOwnUser"},
    {scada::StatusCode::Bad_DuplicateNodeId, "Bad_DuplicateNodeId"},
    {scada::StatusCode::Bad_UnsupportedFileVersion,
     "Bad_UnsupportedFileVersion"},
    {scada::StatusCode::Bad_WrongTypeId, "Bad_WrongTypeId"},
    {scada::StatusCode::Bad_WrongParentId, "Bad_WrongParentId"},
    {scada::StatusCode::Bad_SessionIsLoggedOff, "Bad_SessionIsLoggedOff"},
    {scada::StatusCode::Bad_WrongSubscriptionId, "Bad_WrongSubscriptionId"},
    {scada::StatusCode::Bad_WrongIndex, "Bad_WrongIndex"},
    {scada::StatusCode::Bad_Iec60870UnknownType, "Bad_IecUnknownType"},
    {scada::StatusCode::Bad_Iec60870UnknownCot, "Bad_IecUnknownCot"},
    {scada::StatusCode::Bad_Iec60870UnknownDevice, "Bad_IecUnknownDevice"},
    {scada::StatusCode::Bad_Iec60870UnknownAddress, "Bad_IecUnknownAddress"},
    {scada::StatusCode::Bad_Iec60870UnknownError, "Bad_IecUnknownError"},
    {scada::StatusCode::Bad_WrongCallArguments, "Bad_WrongCallArguments"},
    {scada::StatusCode::Bad_CantParseString, "Bad_CantParseString"},
    {scada::StatusCode::Bad_TooLongString, "Bad_TooLongString"},
    {scada::StatusCode::Bad_WrongPropertyId, "Bad_WrongPropertyId"},
    {scada::StatusCode::Bad_WrongReferenceId, "Bad_WrongReferenceId"},
    {scada::StatusCode::Bad_WrongNodeClass, "Bad_WrongNodeClass"},
    {scada::StatusCode::Bad_WrongAttributeId, "Bad_WrongAttributeId"},
    {scada::StatusCode::Bad_Iec61850Error, "Bad_Iec61850Error"},
    {scada::StatusCode::Bad_NothingToDo, "Bad_NothingToDo"},
    {scada::StatusCode::Bad_BrowseNameInvalid, "Bad_BrowseNameInvalid"},
    {scada::StatusCode::Bad_WrongTargetId, "Bad_WrongTargetId"},
    {scada::StatusCode::Bad_MonitoredItemIdInvalid,
     "Bad_MonitoredItemIdInvalid"},
    {scada::StatusCode::Bad_MessageNotAvailable, "Bad_MessageNotAvailable"},
    {scada::StatusCode::Bad_ApplicationSignatureInvalid,
     "Bad_ApplicationSignatureInvalid"},
    {scada::StatusCode::Bad_TooManyOperations, "Bad_TooManyOperations"},
    {scada::StatusCode::Bad_TooManyMonitoredItems, "Bad_TooManyMonitoredItems"},
    {scada::StatusCode::Bad_SequenceNumberUnknown, "Bad_SequenceNumberUnknown"},
    {scada::StatusCode::Bad_NoContinuationPoints, "Bad_NoContinuationPoints"},
    {scada::StatusCode::Bad_TimestampsToReturnInvalid,
     "Bad_TimestampsToReturnInvalid"},
    {scada::StatusCode::Bad_ViewIdUnknown, "Bad_ViewIdUnknown"},
    {scada::StatusCode::Bad_HistoryOperationInvalid,
     "Bad_HistoryOperationInvalid"},
    {scada::StatusCode::Bad_NoSubscription, "Bad_NoSubscription"},
    {scada::StatusCode::Bad_UserAccessDenied, "Bad_UserAccessDenied"},
    {scada::StatusCode::Bad_NotSupported, "Bad_NotSupported"},
    {scada::StatusCode::Bad_LicenseExpired, "Bad_LicenseExpired"},
    {scada::StatusCode::Bad_WaitingForInitialData, "Bad_WaitingForInitialData"},
    {scada::StatusCode::Bad_OutOfRange, "Bad_OutOfRange"},
    {scada::StatusCode::Bad_NotWritable, "Bad_NotWritable"},
    {scada::StatusCode::Bad_ResponseTooLarge, "Bad_ResponseTooLarge"},
    {scada::StatusCode::Bad_InvalidState, "Bad_InvalidState"},
    {scada::StatusCode::Bad_NotReadable, "Bad_NotReadable"},
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
  if (const scada::StatusTextProvider provider =
          scada::GetStatusTextProvider()) {
    return provider(status_code);
  }

  // No UI layer: the symbolic name. Always ASCII, so widening is lossless.
  const std::string_view name = ToCString(status_code);
  return std::u16string(name.begin(), name.end());
}

std::string ToString(const scada::Status& status) {
  return ToString(status.code());
}

std::u16string ToString16(const scada::Status& status) {
  return ToString16(status.code());
}
