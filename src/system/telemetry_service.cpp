#include "system/telemetry_service.h"

#include "core/log.h"

namespace {
  constexpr Logger kLog("telemetry");
}

void TelemetryService::maybeSend(
    const ConfigService& /*config*/, HttpClient& /*httpClient*/, const WaylandConnection& /*wayland*/
) {
  if (m_sent) {
    return;
  }
  m_sent = true;
  // Motion is local-first. The inherited telemetry setting remains parseable
  // for configuration compatibility but is intentionally ignored.
  kLog.debug("telemetry is disabled permanently in Motion");
}
