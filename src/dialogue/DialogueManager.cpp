#include "DialogueManager.h"

DialogueManager::DialogueManager() {
    loadSampleDialogue();
}

void DialogueManager::loadSampleDialogue() {
    // Nodo 0: Incontro con Gwen
    DialogueNode node0;
    node0.id = 0;
    node0.speaker = "Gwen (Proprietaria Taverna)";
    node0.text = "Benvenuto, viandante della Terra di Nessuno. I 4 Regni si stanno preparando alla guerra...";
    node0.options = {
        {"Cerco di mediare una pace tra i regni.", 1, Realm::Terra, +5},
        {"Non mi importa della guerra, sono qui solo per gli affari.", 2, Realm::Ignis, +5}
    };

    // Nodo 1: Risposta Diplomatica
    DialogueNode node1;
    node1.id = 1;
    node1.speaker = "Gwen";
    node1.text = "Una nobile intenzione. Portare un messaggio di pace nel Regno di Ignis non sara facile.";
    node1.options = {
        {"Accetto la sfida.", -1, Realm::Aer, 0} // -1 chiude il dialogo
    };

    // Nodo 2: Risposta Mercenaria
    DialogueNode node2;
    node2.id = 2;
    node2.speaker = "Gwen";
    node2.text = "I mercenari durano poco in queste terre. Stai attento.";
    node2.options = {
        {"Ho capito.", -1, Realm::Aer, 0}
    };

    m_nodes.push_back(node0);
    m_nodes.push_back(node1);
    m_nodes.push_back(node2);
}

const DialogueNode* DialogueManager::getCurrentNode() const {
    if (m_currentNodeId >= 0 && m_currentNodeId < static_cast<int>(m_nodes.size())) {
        return &m_nodes[m_currentNodeId];
    }
    return nullptr;
}

void DialogueManager::selectOption(int optionIndex, Player& player) {
    const DialogueNode* current = getCurrentNode();
    if (!current || optionIndex < 0 || optionIndex >= static_cast<int>(current->options.size())) return;

    const auto& option = current->options[optionIndex];
    player.modifyReputation(option.affectedRealm, option.reputationChange);

    if (option.nextNodeId == -1) {
        m_active = false; // Fine dialogo
    } else {
        m_currentNodeId = option.nextNodeId;
    }
}