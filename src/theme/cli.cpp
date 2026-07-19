#include "theme/cli.h"

#include "config/config_service.h"
#include "core/files/resource_paths.h"
#include "core/toml.h" // IWYU pragma: keep
#include "theme/builtin_templates.h"
#include "theme/community_templates.h"
#include "theme/image_loader.h"
#include "theme/json_output.h"
#include "theme/palette_generator.h"
#include "theme/template_engine.h"
#include "util/file_utils.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <optional>
#include <print>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

namespace motion::theme {

  namespace {

    constexpr const char* kHelpText =
        "Usage: motion theme <image> [options]\n"
        "       motion theme --list-templates [-c <file>]\n"
        "\n"
        "Generate a wallpaper-derived Material 3 Expressive palette.\n"
        "\n"
        "Options:\n"
        "  --dark            Emit only the dark variant (default)\n"
        "  --light           Emit only the light variant\n"
        "  --both            Emit both variants under dark/light keys\n"
        "  -o <file>         Write JSON to file instead of stdout\n"
        "  -r <in:out>       Render a template file to an output path\n"
        "  -c <file>         Process a TOML template config file\n"
        "  --builtin-config  Process the shipped built-in template catalog\n"
        "  --list-templates  List built-in, cached community, and configured user templates\n"
        "                    Use -c <file> to include a specific template config\n"
        "  --default-mode    Template default mode: dark or light";

    std::filesystem::path builtinTemplateConfigPath() { return paths::assetPath("templates/builtin.toml"); }

    struct TemplateListEntry {
      std::string id;
      std::string category;
      std::string name;
    };

    std::string templateNameOrId(const std::string& id, const std::string& name) { return name.empty() ? id : name; }

    void sortTemplateList(std::vector<TemplateListEntry>& entries) {
      std::ranges::sort(entries, [](const TemplateListEntry& lhs, const TemplateListEntry& rhs) {
        return std::tie(lhs.category, lhs.id, lhs.name) < std::tie(rhs.category, rhs.id, rhs.name);
      });
    }

    std::vector<TemplateListEntry> loadBuiltinTemplateList(std::string& err) {
      std::vector<TemplateListEntry> out;
      const auto builtins = motion::theme::loadBuiltinTemplateInfo(&err);
      out.reserve(builtins.size());
      for (const auto& builtin : builtins) {
        out.push_back(
            TemplateListEntry{
                .id = builtin.id,
                .category = builtin.category,
                .name = templateNameOrId(builtin.id, builtin.name),
            }
        );
      }
      sortTemplateList(out);
      return out;
    }

    std::vector<TemplateListEntry> loadCommunityTemplateList() {
      std::vector<TemplateListEntry> out;
      const auto community = CommunityTemplateService::availableTemplates();
      out.reserve(community.size());
      for (const auto& entry : community) {
        out.push_back(
            TemplateListEntry{
                .id = entry.id,
                .category = entry.category,
                .name = templateNameOrId(entry.id, entry.displayName),
            }
        );
      }
      sortTemplateList(out);
      return out;
    }

    std::vector<TemplateListEntry> loadConfiguredUserTemplateList() {
      ConfigService config;
      const auto& userTemplates = config.config().theme.templates.userTemplates;

      std::vector<TemplateListEntry> out;
      out.reserve(userTemplates.size());
      for (const auto& entry : userTemplates) {
        out.push_back(
            TemplateListEntry{
                .id = entry.id,
                .category = "user",
                .name = entry.id,
            }
        );
      }
      sortTemplateList(out);
      return out;
    }

    std::unordered_map<std::string, TemplateListEntry> loadTemplateCatalog(const toml::table& root) {
      std::unordered_map<std::string, TemplateListEntry> out;
      const toml::table* catalog = root["catalog"].as_table();
      if (catalog == nullptr)
        return out;

      for (const auto& [idNode, node] : *catalog) {
        const auto id = std::string(idNode.str());
        TemplateListEntry entry{.id = id, .category = {}, .name = id};
        if (const toml::table* info = node.as_table()) {
          if (const auto name = info->get_as<std::string>("name"))
            entry.name = name->get();
          if (const auto category = info->get_as<std::string>("category"))
            entry.category = category->get();
        }
        out[id] = std::move(entry);
      }
      return out;
    }

    std::vector<TemplateListEntry>
    loadTemplateConfigList(const std::filesystem::path& path, bool required, std::string& err) {
      std::error_code ec;
      if (!std::filesystem::exists(path, ec)) {
        if (required)
          err = "file does not exist";
        return {};
      }

      toml::table root;
      try {
        root = toml::parse_file(path.string());
      } catch (const toml::parse_error& e) {
        err = e.description();
        return {};
      }

      std::vector<TemplateListEntry> out;
      const toml::table* templates = root["templates"].as_table();
      if (templates == nullptr)
        return out;

      const auto catalog = loadTemplateCatalog(root);
      out.reserve(templates->size());
      for (const auto& [idNode, node] : *templates) {
        if (node.as_table() == nullptr)
          continue;
        const auto id = std::string(idNode.str());
        auto catalogIt = catalog.find(id);
        if (catalogIt != catalog.end()) {
          out.push_back(catalogIt->second);
        } else {
          out.push_back(TemplateListEntry{.id = id, .category = {}, .name = id});
        }
      }
      sortTemplateList(out);
      return out;
    }

    void printTemplateListGroup(const char* title, const std::vector<TemplateListEntry>& entries, bool& firstGroup) {
      if (entries.empty())
        return;

      if (!firstGroup)
        std::println();
      firstGroup = false;
      std::println("{}", title);

      std::size_t idWidth = std::strlen("ID");
      std::size_t categoryWidth = std::strlen("Category");
      for (const auto& entry : entries) {
        idWidth = std::max(idWidth, entry.id.size());
        categoryWidth = std::max(categoryWidth, entry.category.empty() ? std::size_t{1} : entry.category.size());
      }

      const auto idColumn = static_cast<int>(idWidth);
      const auto categoryColumn = static_cast<int>(categoryWidth);
      std::println("  {:{}}  {:{}}  {}", "ID", idColumn, "Category", categoryColumn, "Name");
      for (const auto& entry : entries) {
        const std::string category = entry.category.empty() ? "-" : entry.category;
        std::println("  {:{}}  {:{}}  {}", entry.id, idColumn, category, categoryColumn, entry.name);
      }
    }

    int listTemplates(const char* configPath) {
      std::string err;
      const auto builtins = loadBuiltinTemplateList(err);
      if (!err.empty()) {
        std::println(stderr, "error: failed to load built-in templates: {}", err);
        return 1;
      }

      const auto community = loadCommunityTemplateList();
      std::vector<TemplateListEntry> userTemplates;
      std::string explicitConfigTitle = "Template config";
      if (configPath != nullptr) {
        const std::filesystem::path templateConfigPath = FileUtils::expandUserPath(configPath);
        std::string userErr;
        userTemplates = loadTemplateConfigList(templateConfigPath, true, userErr);
        if (!userErr.empty()) {
          std::println(stderr, "error: failed to load template config {}: {}", templateConfigPath.string(), userErr);
          return 1;
        }
        explicitConfigTitle += " (";
        explicitConfigTitle += templateConfigPath.filename().string();
        explicitConfigTitle += ")";
      } else {
        userTemplates = loadConfiguredUserTemplateList();
      }

      bool firstGroup = true;
      printTemplateListGroup("Built-in templates", builtins, firstGroup);
      printTemplateListGroup("Community templates (cached)", community, firstGroup);
      printTemplateListGroup(
          configPath != nullptr ? explicitConfigTitle.c_str() : "User templates", userTemplates, firstGroup
      );
      if (firstGroup)
        std::println("No templates found.");
      return 0;
    }

  } // namespace

  int runCli(int argc, char* argv[]) {
    const char* imagePath = nullptr;
    Variant variant = Variant::Dark;
    const char* outPath = nullptr;
    const char* configPath = nullptr;
    std::string builtinConfigPathStorage;
    bool builtinConfig = false;
    bool listTemplatesRequested = false;
    std::string defaultMode = "dark";
    std::vector<std::string> renderSpecs;

    for (int i = 2; i < argc; ++i) {
      const char* a = argv[i];
      if (std::strcmp(a, "--help") == 0) {
        std::println("{}", kHelpText);
        return 0;
      }
      if (std::strcmp(a, "--dark") == 0) {
        variant = Variant::Dark;
        continue;
      }
      if (std::strcmp(a, "--light") == 0) {
        variant = Variant::Light;
        continue;
      }
      if (std::strcmp(a, "--both") == 0) {
        variant = Variant::Both;
        continue;
      }
      if (std::strcmp(a, "-o") == 0 && i + 1 < argc) {
        outPath = argv[++i];
        continue;
      }
      if ((std::strcmp(a, "--render") == 0 || std::strcmp(a, "-r") == 0) && i + 1 < argc) {
        renderSpecs.emplace_back(argv[++i]);
        continue;
      }
      if ((std::strcmp(a, "--config") == 0 || std::strcmp(a, "-c") == 0) && i + 1 < argc) {
        configPath = argv[++i];
        continue;
      }
      if (std::strcmp(a, "--builtin-config") == 0) {
        builtinConfig = true;
        continue;
      }
      if (std::strcmp(a, "--list-templates") == 0) {
        listTemplatesRequested = true;
        continue;
      }
      if (std::strcmp(a, "--default-mode") == 0 && i + 1 < argc) {
        defaultMode = argv[++i];
        continue;
      }
      if (!imagePath && a[0] != '-') {
        imagePath = a;
        continue;
      }
      std::println(stderr, "error: unknown theme argument: {}", a);
      return 1;
    }

    if (listTemplatesRequested)
      return listTemplates(configPath);

    if (builtinConfig) {
      if (configPath != nullptr) {
        std::println(stderr, "error: --builtin-config cannot be combined with --config");
        return 1;
      }
      builtinConfigPathStorage = builtinTemplateConfigPath().string();
      configPath = builtinConfigPathStorage.c_str();
    }

    if (!imagePath) {
      std::println(stderr, "error: theme requires a wallpaper image path (try: motion theme --help)");
      return 1;
    }

    GeneratedPalette palette;
    auto loaded = loadAndResize(imagePath);
    if (!loaded) {
      std::println(stderr, "error: failed to load image: {}", loaded.error());
      return 1;
    }

    auto generated = generate(loaded->rgb);
    if (!generated) {
      std::println(stderr, "error: M3 Expressive palette generation failed: {}", generated.error());
      return 1;
    }
    palette = std::move(*generated);

    const std::string json = toJson(palette, variant);
    const bool hasTemplateWork = !renderSpecs.empty() || configPath != nullptr;
    if (outPath) {
      std::ofstream f(outPath);
      if (!f) {
        std::println(stderr, "error: cannot open output file: {}", outPath);
        return 1;
      }
      f << json << '\n';
    } else if (!hasTemplateWork) {
      std::fwrite(json.data(), 1, json.size(), stdout);
      std::fputc('\n', stdout);
    }

    if (hasTemplateWork) {
      TemplateEngine::Options options;
      options.defaultMode = defaultMode;
      options.imagePath = imagePath ? imagePath : "";
      options.closestColor.clear();
      options.verbose = true;
      TemplateEngine engine(TemplateEngine::makeThemeData(palette), std::move(options));

      for (const auto& spec : renderSpecs) {
        const size_t colon = spec.find(':');
        if (colon == std::string::npos) {
          std::println(stderr, "error: invalid render spec (expected input:output): {}", spec);
          return 1;
        }
        const std::filesystem::path input = FileUtils::expandUserPath(spec.substr(0, colon));
        const std::filesystem::path output = FileUtils::expandUserPath(spec.substr(colon + 1));
        const auto result = engine.renderFile(input, output);
        if (!result.success)
          return 1;
      }

      if (configPath && !engine.processConfigFile(configPath))
        return 1;
    }

    return 0;
  }

} // namespace motion::theme
