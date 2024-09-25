#pragma once
#include "Patch.h"
#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;
using std::string;

namespace Patch {

    std::map<string,std::vector<std::pair<string,string>>> renames = {};

    inline bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if(start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }

    void Setup() {
        string configFileName = std::format("Data/SKSE/Plugins/{}.json", Plugin::NAME);
        logger::info("Loading config file {}", configFileName);
        std::ifstream configFile(configFileName);
        if (!configFile.good()) {
            logger::error("Config file {} not found", configFileName);
            return;
        }
        json config = json::parse(configFile);
        logger::info("config: {}", nlohmann::to_string(config));
        for (auto& [key,value] : config.items()) {
            for(auto& entry : value) {
                auto from = entry.at(0).template get<string>();
                auto to =  entry.at(1).template get<string>();
                logger::info("{} Entry: {} -> {}", key, from, to);
                renames[key].push_back(std::make_pair(from,to));
            }
        }
    }

    inline bool rename(RE::TESFullName* obj, string type) {
        bool ret = false;
        string name = obj->GetFullName();
        for(auto rename : renames[type]) {
            string oldName = name;
            if (replace(name, rename.first, rename.second)) {
                logger::info("Renamed {} -> {}", oldName, name);
                obj->SetFullName(name.c_str());
                ret = true;
            }
        }
        return ret;
    }

    template<typename T>
    void renameAll(string type) {
        const auto HANDLER = RE::TESDataHandler::GetSingleton();
        auto things = HANDLER->GetFormArray<T>();
        if (renames.contains(type)) {
            logger::info("Found {} entries of type {}", things.size(), type);
            for (auto * thing : things) {
                if (!thing) {
                    logger::info("Thing was null");
                    continue;
                }
                rename(thing,type);
            }
        }
    }

    void ProcessLoadOrder() {

        renameAll<RE::TESObjectARMO>("Armor");
        renameAll<RE::TESObjectWEAP>("Weapon");
        renameAll<RE::IngredientItem>("Ingredient");
        renameAll<RE::AlchemyItem>("Ingestible");
        renameAll<RE::TESObjectACTI>("Activator");
        renameAll<RE::TESFlora>("Flora");
        renameAll<RE::TESObjectMISC>("Misc");
        renameAll<RE::TESFurniture>("Furniture");
        renameAll<RE::TESObjectDOOR>("Door");
        renameAll<RE::TESObjectCONT>("Container");


    }
}