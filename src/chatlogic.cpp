#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <iterator>
#include <tuple>
#include <algorithm>

#include "graphedge.h"
#include "graphnode.h"
#include "chatbot.h"
#include "chatlogic.h"

using namespace std;

ChatLogic::ChatLogic()
{
    //// STUDENT CODE TODO
    ////

    // create instance of chatbot
    _chatBot = new ChatBot("../images/chatbot.png");

    // add pointer to chatlogic so that chatbot answers can be passed on to the GUI
    _chatBot->SetChatLogicHandle(this);

    ////
    //// EOF STUDENT CODE
}

ChatLogic::~ChatLogic()
{
    //// STUDENT CODE TODO
    ////

    // delete chatbot instance
    delete _chatBot;

    // delete all nodes 
    // removed in task 3

    // delete all edges
    // removed in task 4

    ////
    //// EOF STUDENT CODE
}

template <typename T>
void ChatLogic::AddAllTokensToElement(string tokenID, tokenlist &tokens, T &element)
{
    // find all occurences for current node
    auto token = tokens.begin();
    while (true)
    {
        token = find_if(token, tokens.end(), [&tokenID](const pair<string, string> &pair) { return pair.first == tokenID;; });
        if (token != tokens.end())
        {
            element.AddToken(token->second); // add new keyword to edge
            token++;                         // increment iterator to next element
        }
        else
        {
            break; // quit infinite while-loop
        }
    }
}

void ChatLogic::LoadAnswerGraphFromFile(string filename)
{
    // load file with answer graph elements
    ifstream file(filename);

    // check for file availability and process it line by line
    if (file)
    {
        // loop over all lines in the file
        string lineStr;
        while (getline(file, lineStr))
        {
            // extract all tokens from current line
            tokenlist tokens;
            while (lineStr.size() > 0)
            {
                // extract next token
                int posTokenFront = lineStr.find("<");
                int posTokenBack = lineStr.find(">");
                if (posTokenFront < 0 || posTokenBack < 0)
                    break; // quit loop if no complete token has been found
                string tokenStr = lineStr.substr(posTokenFront + 1, posTokenBack - 1);

                // extract token type and info
                int posTokenInfo = tokenStr.find(":");
                if (posTokenInfo != string::npos)
                {
                    string tokenType = tokenStr.substr(0, posTokenInfo);
                    string tokenInfo = tokenStr.substr(posTokenInfo + 1, tokenStr.size() - 1);

                    // add token to vector
                    tokens.push_back(make_pair(tokenType, tokenInfo));
                }

                // remove token from current line
                lineStr = lineStr.substr(posTokenBack + 1, lineStr.size());
            }

            // process tokens for current line
            auto type = find_if(tokens.begin(), tokens.end(), [](const pair<string, string> &pair) { return pair.first == "TYPE"; });
            if (type != tokens.end())
            {
                // check for id
                auto idToken = find_if(tokens.begin(), tokens.end(), [](const pair<string, string> &pair) { return pair.first == "ID"; });
                if (idToken != tokens.end())
                {
                    // extract id from token
                    int id = stoi(idToken->second);

                    // node-based processing
                    if (type->second == "NODE")
                    {
                        //// STUDENT CODE TODO
                        ////

                        // check if node with this ID exists already
                        auto newNode = find_if(_nodes.begin(), _nodes.end(), [&id](unique_ptr<GraphNode> &node) { return node->GetID() == id; });

                        // create new element if ID does not yet exist
                        if (newNode == _nodes.end())
                        {
                            _nodes.emplace_back(make_unique<GraphNode>(id));
                            newNode = _nodes.end() - 1; // get iterator to last element

                            // add all answers to current node
                            AddAllTokensToElement("ANSWER", tokens, **newNode);
                        }

                        ////
                        //// EOF STUDENT CODE
                    }

                    // edge-based processing
                    if (type->second == "EDGE")
                    {
                        //// STUDENT CODE TODO
                        ////

                        // find tokens for incoming (parent) and outgoing (child) node
                        auto parentToken = find_if(tokens.begin(), tokens.end(), [](const pair<string, string> &pair) { return pair.first == "PARENT"; });
                        auto childToken = find_if(tokens.begin(), tokens.end(), [](const pair<string, string> &pair) { return pair.first == "CHILD"; });

                        if (parentToken != tokens.end() && childToken != tokens.end())
                        {
                            // get iterator on incoming and outgoing node via ID search
                            auto parentNode = find_if(_nodes.begin(), _nodes.end(), [&parentToken](const unique_ptr<GraphNode> &node) { return node->GetID() == stoi(parentToken->second); });
                            auto childNode = find_if(_nodes.begin(), _nodes.end(), [&childToken](const unique_ptr<GraphNode> &node) { return node->GetID() == stoi(childToken->second); });

                            // create new edge
                            unique_ptr<GraphEdge> edge = make_unique<GraphEdge>(id);
                            edge->SetChildNode((*childNode).get());
                            edge->SetParentNode((*parentNode).get());

                            // find all keywords for current node
                            AddAllTokensToElement("KEYWORD", tokens, *edge);

                            // store reference in child node and parent node
                            (*childNode)->AddEdgeToParentNode(edge.get());
                            (*parentNode)->AddEdgeToChildNode(move(edge));
                        }

                        ////
                        //// EOF STUDENT CODE
                    }
                }
                else
                {
                    cout << "Error: ID missing. Line is ignored!" << endl;
                }
            }
        } // eof loop over all lines in the file

        file.close();

    } // eof check for file availability
    else
    {
        cout << "File could not be opened!" << endl;
        return;
    }

    //// STUDENT CODE TODO
    ////

    // identify root node
    GraphNode *rootNode = nullptr;
    for (auto it = begin(_nodes); it != end(_nodes); ++it)
    {
        // search for nodes which have no incoming edges
        if ((*it)->GetNumberOfParents() == 0)
        {

            if (rootNode == nullptr)
            {
                rootNode = it->get();; // assign current node to root
            }
            else
            {
                cout << "ERROR : Multiple root nodes detected" << endl;
            }
        }
    }

    // add chatbot to graph root node
    ChatBot localCB = ChatBot("../images/chatbot.png");
    localCB.SetChatLogicHandle(this);
    localCB.SetRootNode(rootNode);
    rootNode->MoveChatbotHere(move(localCB));
    _chatBot->SetRootNode(rootNode);
    
    ////
    //// EOF STUDENT CODE
}

void ChatLogic::SetPanelDialogHandle(ChatBotPanelDialog *panelDialog)
{
    _panelDialog = panelDialog;
}

void ChatLogic::SetChatbotHandle(ChatBot *chatbot)
{
    _chatBot = chatbot;
}

void ChatLogic::SendMessageToChatbot(string message)
{
    _chatBot->ReceiveMessageFromUser(message);
}

void ChatLogic::SendMessageToUser(string message)
{
    _panelDialog->PrintChatbotResponse(message);
}

wxBitmap *ChatLogic::GetImageFromChatbot()
{
    return _chatBot->GetImageHandle();
}