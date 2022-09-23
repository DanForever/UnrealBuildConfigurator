
#include <pugixml.hpp>
#include <CLI11.hpp>
#include <vector>
#include <string>
#include <filesystem>

enum class ParameterValue
{
	NotSet = 0,
	Enable,
	Disable,
};

struct ConfigurationOption
{
	// The name of the option that goes in the xml file
	const char* Name;

	// Various different ways of specifying this option on the command line
	std::vector<const char*> ParameterAliases;

	ParameterValue Value = ParameterValue::NotSet;
};

std::vector<ConfigurationOption> ConfigurationOptions =
{
	{ "bAllowXGE", { "incredibuild", "xge", "bAllowXGE" } },
	{ "bAllowSNDBS", { "sndbs", "bAllowSNDBS" } },
	{ "bAllowFASTBuild", { "fastbuild", "bAllowFASTBuild" } },
	{ "bUseUnityBuild", { "unitybuild", "unity", "bUseUnityBuild" } },
};

std::string ConstructOptionString(const ConfigurationOption& config)
{
	bool first = true;

	std::ostringstream output;
	for (const char* alias : config.ParameterAliases)
	{
		if (first)
		{
			first = false;
		}
		else
		{
			output << ",";
		}

		output << "--";
		output << alias;
		output << "{";
		output << (int)ParameterValue::Enable;
		output << "},";

		output << "!--no-";
		output << alias;
		output << "{";
		output << (int)ParameterValue::Disable;
		output << "}";
	}

	return output.str();
}

std::filesystem::path FindPathToBuildXml(const std::string input)
{
	std::filesystem::path path = input;

	if (!std::filesystem::exists(path))
		return {};

	if (path.filename() == "BuildConfiguration.xml")
		return path;

	if (path.extension() == ".uproject")
	{
		path = path.parent_path().parent_path();
		path = path / "Engine" / "Saved" / "UnrealBuildTool" / "BuildConfiguration.xml";
	}

	if (!std::filesystem::exists(path))
		return {};

	return path;
}

pugi::xml_node FindOrCreateBuildConfigurationNode(pugi::xml_node& parent)
{
	pugi::xml_node configurationNode = parent.child("Configuration");

	if (!configurationNode)
	{
		configurationNode = parent.append_child("Configuration");
		configurationNode.append_attribute("xmlns").set_value("https://www.unrealengine.com/BuildConfiguration");
	}

	pugi::xml_node buildConfigurationNode = configurationNode.child("BuildConfiguration");

	if (!buildConfigurationNode)
	{
		buildConfigurationNode = configurationNode.append_child("BuildConfiguration");
	}

	return buildConfigurationNode;
}

pugi::xml_node FindOrCreate(pugi::xml_node& parent, const pugi::char_t* name)
{
	pugi::xml_node node = parent.child(name);

	if (!node)
	{
		node = parent.append_child(name);
	}

	return node;
}

void SetValue(pugi::xml_node& node, const pugi::char_t* value)
{
	pugi::xml_node valueNode = node.first_child();

	if (!valueNode)
	{
		valueNode = node.append_child(pugi::node_pcdata);
	}

	valueNode.set_value(value);
}

void FindOrCreateAndSetValue(pugi::xml_node& parent, const pugi::char_t* name, const pugi::char_t* value)
{
	pugi::xml_node node = FindOrCreate(parent, name);
	SetValue(node, value);
}

int main(int argc, char** argv)
{
	CLI::App app{ "Quick and easy configuration of Unreal's BuildConfiguration.xml", "Dan Forever's Unreal Build Configurator" };

	std::string filename = "";
	app.add_option("-f,--file", filename, "The XML file to modify");

	for (auto& config : ConfigurationOptions)
	{
		std::string optionString = ConstructOptionString(config);

		app.add_flag(optionString, config.Value);
	}

	CLI11_PARSE(app, argc, argv);

	std::filesystem::path path = FindPathToBuildXml(filename);
	if (path.empty())
		return 1;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(path.c_str());
	if (!result)
		return -2;

	bool changed = false;
	pugi::xml_node buildConfigurationNode = FindOrCreateBuildConfigurationNode(doc);

	for (const auto& config : ConfigurationOptions)
	{
		switch (config.Value)
		{
		case ParameterValue::Enable:
			FindOrCreateAndSetValue(buildConfigurationNode, config.Name, "true");
			changed = true;
			break;

		case ParameterValue::Disable:
			FindOrCreateAndSetValue(buildConfigurationNode, config.Name, "false");
			changed = true;
			break;
		}
	}

	if (changed)
	{
		doc.save_file(path.c_str());
	}

	return 0;
}
