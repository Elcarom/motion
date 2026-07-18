#include "config/config_migrations.h"
#include "core/toml.h"

#include <cstdint>
#include <format>
#include <limits>
#include <print>
#include <string>
#include <string_view>

namespace {

  int g_failures = 0;
  int g_syntheticMigrationApplications = 0;

  void countSyntheticMigration(toml::table&, motion::config::schema::Diagnostics&) {
    ++g_syntheticMigrationApplications;
  }

  void expect(bool condition, std::string_view message) {
    if (!condition) {
      std::println(stderr, "config_migration_test: FAIL: {}", message);
      ++g_failures;
    }
  }

  void checkNegativeRadiusMigration() {
    toml::table root = toml::parse(R"(
[bar.main]
radius = -12
radius_top_left = -20
radius_top_right = 8

[bar.main.monitor.dp1]
match = "DP-1"
radius = -16

[dock]
radius = -7
)");

    motion::config::LegacyConfigIssues issues;
    motion::config::normalizeLegacyConfig(root, issues);

    expect(root["bar"]["main"]["radius"].value<std::int64_t>() == 12, "base radius was not made positive");
    expect(
        root["bar"]["main"]["radius_top_left"].value<std::int64_t>() == 20,
        "per-corner radius was not made positive"
    );
    expect(
        root["bar"]["main"]["radius_top_right"].value<std::int64_t>() == 8,
        "positive radius was changed"
    );
    expect(
        root["bar"]["main"]["concave_edge_corners"].value<bool>() == true,
        "base concave flag was not set"
    );
    expect(
        root["bar"]["main"]["monitor"]["dp1"]["radius"].value<std::int64_t>() == 16,
        "monitor radius was not migrated"
    );
    expect(
        root["bar"]["main"]["monitor"]["dp1"]["concave_edge_corners"].value<bool>() == true,
        "monitor concave flag was not set"
    );
    expect(root["dock"]["radius"].value<std::int64_t>() == -7, "dock radius was incorrectly migrated");
    expect(issues.size() == 2, "expected one issue for the bar and one for its monitor override");

    motion::config::LegacyConfigIssues secondPassIssues;
    motion::config::normalizeLegacyConfig(root, secondPassIssues);
    expect(secondPassIssues.empty(), "normalization was not idempotent");
  }

  void checkExtremeNegativeRadius() {
    toml::table root;
    toml::table bar;
    bar.insert_or_assign("radius", std::numeric_limits<std::int64_t>::min());
    toml::table bars;
    bars.insert_or_assign("main", std::move(bar));
    root.insert_or_assign("bar", std::move(bars));

    motion::config::LegacyConfigIssues issues;
    motion::config::normalizeLegacyConfig(root, issues);
    expect(
        root["bar"]["main"]["radius"].value<std::int64_t>() == 500,
        "extreme negative radius did not normalize safely"
    );
  }

  void checkCustomScheduleMigration() {
    toml::table legacy = toml::parse(R"(
[location]
sunset = "20:30"
sunrise = "07:30"
)");
    motion::config::LegacyConfigIssues issues;
    motion::config::normalizeLegacyConfig(legacy, issues);
    expect(
        legacy["location"]["custom_schedule"].value<bool>() == true,
        "a times-only location did not opt into custom scheduling"
    );
    expect(issues.size() == 1, "times-only location did not report a legacy issue");

    motion::config::LegacyConfigIssues secondPassIssues;
    motion::config::normalizeLegacyConfig(legacy, secondPassIssues);
    expect(secondPassIssues.empty(), "custom scheduling normalization was not idempotent");

    // Coordinates won under the old rules, so these configs must keep using them.
    for (const std::string_view source : {"auto_locate = true", "address = \"Toronto, ON\"",
                                          "latitude = 52.52\nlongitude = 13.405"}) {
      toml::table coords = toml::parse(std::format("[location]\nsunset = \"20:30\"\nsunrise = \"07:30\"\n{}\n", source));
      motion::config::LegacyConfigIssues coordIssues;
      motion::config::normalizeLegacyConfig(coords, coordIssues);
      expect(
          !coords["location"]["custom_schedule"].value<bool>().has_value(),
          "a location with coordinates was switched to custom scheduling"
      );
      expect(coordIssues.empty(), "a location with coordinates reported a legacy issue");
    }

    toml::table explicitOff = toml::parse(R"(
[location]
custom_schedule = false
sunset = "20:30"
sunrise = "07:30"
)");
    motion::config::LegacyConfigIssues offIssues;
    motion::config::normalizeLegacyConfig(explicitOff, offIssues);
    expect(
        explicitOff["location"]["custom_schedule"].value<bool>() == false,
        "an explicit custom_schedule = false was overwritten"
    );
  }

  void checkVersionGating() {
    toml::table legacy = toml::parse(R"(
[bar.main]
radius = -10
)");
    motion::config::schema::Diagnostics diag;
    const auto stored = motion::config::storedConfigVersion(legacy, diag);
    expect(stored == 0, "missing config_version was not treated as legacy version 0");
    const int applied = motion::config::applyPendingConfigMigrations(legacy, stored.value_or(0), diag);
    expect(applied == motion::config::currentConfigVersion(), "pending migration did not reach current version");
    expect(legacy["bar"]["main"]["radius"].value<std::int64_t>() == 10, "sidecar migration did not run");

    toml::table current = toml::parse(R"(
config_version = 1
[bar.main]
radius = -10
)");
    motion::config::schema::Diagnostics currentDiag;
    const auto currentStored = motion::config::storedConfigVersion(current, currentDiag);
    expect(currentStored == 1, "current config_version was not read");
    (void)motion::config::applyPendingConfigMigrations(current, currentStored.value_or(0), currentDiag);
    expect(
        current["bar"]["main"]["radius"].value<std::int64_t>() == -10,
        "current sidecar replayed a historical migration"
    );

    toml::table invalid = toml::parse("config_version = \"one\"");
    motion::config::schema::Diagnostics invalidDiag;
    expect(
        !motion::config::storedConfigVersion(invalid, invalidDiag).has_value(),
        "invalid config_version was accepted"
    );
    expect(invalidDiag.hasErrors(), "invalid config_version did not produce an error");
    expect(invalidDiag.hasFatalErrors(), "invalid config_version was not document-fatal");

    toml::table future = toml::parse("config_version = 999");
    motion::config::schema::Diagnostics futureDiag;
    expect(
        !motion::config::storedConfigVersion(future, futureDiag).has_value(),
        "future config_version was accepted"
    );
    expect(futureDiag.hasErrors(), "future config_version did not produce an error");
    expect(futureDiag.hasFatalErrors(), "future config_version was not document-fatal");

    motion::config::schema::Diagnostics baseline;
    baseline.componentError("widget.clock.timezone", "widget.clock", "unknown timezone", "clock.timezone.unknown");
    motion::config::schema::Diagnostics candidate = baseline;
    candidate.error("accessibility.ui_scale", "expected a number", "config.type.number");
    const auto introduced = candidate.introducedErrorsComparedTo(baseline);
    expect(introduced.entries.size() == 1, "diagnostic comparison did not isolate the new error");
    expect(introduced.entries.front().path == "accessibility.ui_scale", "diagnostic comparison returned the wrong error");
  }

  void checkReminderFingerprint() {
    const motion::config::LegacyConfigIssues first = {{1, "bar.main", "message"}};
    const motion::config::LegacyConfigIssues reordered = {
        {1, "bar.second", "message"},
        {1, "bar.main", "different display message"},
    };
    const motion::config::LegacyConfigIssues sameReordered = {
        {1, "bar.main", "message"},
        {1, "bar.second", "message"},
    };

    const std::string firstFingerprint = motion::config::legacyConfigIssueFingerprint(first);
    const std::string expandedFingerprint = motion::config::legacyConfigIssueFingerprint(reordered);
    expect(
        expandedFingerprint == motion::config::legacyConfigIssueFingerprint(sameReordered),
        "fingerprint depends on issue ordering or display message"
    );
    expect(
        motion::config::legacyConfigFingerprintHasNewIssues(expandedFingerprint, firstFingerprint),
        "new issue was not detected"
    );
    expect(
        !motion::config::legacyConfigFingerprintHasNewIssues(firstFingerprint, expandedFingerprint),
        "removing an issue was treated as introducing one"
    );

    constexpr std::int64_t kStart = 1'000'000;
    expect(
        !motion::config::legacyConfigReminderIntervalElapsed(
            kStart + motion::config::kLegacyConfigReminderIntervalSeconds - 1, kStart
        ),
        "reminder became due before three days"
    );
    expect(
        motion::config::legacyConfigReminderIntervalElapsed(
            kStart + motion::config::kLegacyConfigReminderIntervalSeconds, kStart
        ),
        "reminder was not due at three days"
    );
    expect(
        motion::config::legacyConfigReminderIntervalElapsed(kStart - 1, kStart),
        "backward clock change did not make the reminder due"
    );
  }

  void checkRegistryOrdering() {
    int expectedVersion = 1;
    for (const auto& migration : motion::config::configMigrations()) {
      expect(migration.toVersion == expectedVersion, "migration registry has a gap or is out of order");
      expect(!migration.summary.empty(), "migration registry entry has no summary");
      expect(migration.apply != nullptr, "migration registry entry has no apply function");
      ++expectedVersion;
    }
    expect(
        expectedVersion - 1 == motion::config::currentConfigVersion(),
        "current config version does not match the registry"
    );
  }

  void checkLargeCurrentRegistrySkipsBodies() {
    std::vector<motion::config::ConfigMigration> migrations;
    migrations.reserve(100);
    for (int version = 1; version <= 100; ++version) {
      migrations.push_back({
          .toVersion = version,
          .summary = "synthetic migration",
          .apply = countSyntheticMigration,
      });
    }

    toml::table root;
    motion::config::schema::Diagnostics diag;
    g_syntheticMigrationApplications = 0;
    const int current = motion::config::applyPendingConfigMigrations(root, 100, diag, migrations);
    expect(current == 100, "synthetic current version changed");
    expect(g_syntheticMigrationApplications == 0, "current sidecar executed historical migration bodies");

    const int upgraded = motion::config::applyPendingConfigMigrations(root, 99, diag, migrations);
    expect(upgraded == 100, "synthetic upgrade did not reach the current version");
    expect(g_syntheticMigrationApplications == 1, "synthetic upgrade did not execute exactly one pending body");
  }

} // namespace

int main() {
  checkNegativeRadiusMigration();
  checkExtremeNegativeRadius();
  checkCustomScheduleMigration();
  checkVersionGating();
  checkReminderFingerprint();
  checkRegistryOrdering();
  checkLargeCurrentRegistrySkipsBodies();

  if (g_failures == 0) {
    std::println("config_migration_test: all checks passed");
  }
  return g_failures == 0 ? 0 : 1;
}
