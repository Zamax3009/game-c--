#ifndef DIALOGUE_MANAGER_H
#define DIALOGUE_MANAGER_H
#include <string>
#include <vector>
#include "../entities/Player.h"

struct DialogueOption {
    std::string text;
    int nextNodeId;
    Realm affectedRealm;
    int reputationChange;
};

struct DialogueNode {
    int id;
    std::string speaker;
    std::string text;
    std::vector<DialogueOption> options;
};

class DialogueManager{
public:
    DialogueManager();
    const DialogueNode* getCurrentNode() const;
    bool loadFromFile(const std::string& FilePath);
    void selectOption(int optionsIndex, Player& player);
    bool isDialogueActive() const {return m_active; }
    void startDialogue(int startNodeId) { m_currentNodeId = startNodeId; m_active = true; }

private:
    std::vector<DialogueNode> m_nodes;
    int m_currentNodeId = 0;
    bool m_active = false;
};


#endif