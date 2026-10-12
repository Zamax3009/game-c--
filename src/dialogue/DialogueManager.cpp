#include "DialogueManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

static Realm stringToRealm(const std::string& str){
    if (str == "Aer") return Realm::Aer;
    if (str == "Aqua") return Realm::Aqua;
    if (str == "Terra") return Realm::Terra;
    if (str == "Ignis") return Realm::Ignis;
    return Realm::Aer;
}

bool DialogueManager::loadFromFile(const std::string& FilePath, const std::string& npcKey){
    std::ifstream file(FilePath);
    if(!file.is_open()){
        std::cerr << "Error: couldn't open the file " << FilePath << std::endl;
        return false;
    }

    try {
        json j;
        file >> j;

        m_nodes.clear();

        const auto& npcNode = j.at(npcKey);

        for (const auto& item : j.at(npcKey)) {
            DialogueNode node;
            node.id = item.at("id").get<int>();
            node.speaker = item.at("speaker").get<std::string>();
            node.text = item.at("text").get<std::string>();
            for (const auto& opt : item.at("options")){
                DialogueOption option;
                option.text = opt.at("text").get<std::string>();
                option.nextNodeId = opt.at("nextNodeId").get<int>();
                option.affectedRealm = stringToRealm(opt.at("affectedRealm").get<std::string>());
                option.reputationChange = opt.at("reputationChange").get<int>();
                option.masteryChange = opt.at("masteryChange").get<int>();
                node.options.push_back(option);
            }
            m_nodes.push_back(node);
        }
        return true;
    }
    catch (const json::exception& e) {
        std::cerr << "JSON parsing error:" << e.what() << std::endl;
        return false;
    }
}

DialogueManager::DialogueManager() {
    loadFromFile("assets/TerraDialogues.json", "gwen");
}

const DialogueNode* DialogueManager::getCurrentNode() const {
    for (const auto& node : m_nodes) {
        if (node.id == m_currentNodeId){
            return &node;
        }
    }
    return nullptr;
}

void DialogueManager::selectOption(int optionIndex, Player& player) {
    const DialogueNode* current = getCurrentNode();
    if (current == nullptr || optionIndex < 0 || optionIndex >= static_cast<int>(current->options.size())){
        return;
    };

    const DialogueOption& option = current -> options[optionIndex];
    player.modifyReputation(option.affectedRealm, option.reputationChange);
    player.modifyMastery(option.affectedRealm, option.masteryChange);


    if (option.nextNodeId == -1) {
        m_active = false;
    }
    else {
        m_currentNodeId = option.nextNodeId;
    }
}