#include "graphedge.h"
#include "graphnode.h"

GraphNode::GraphNode(int id)
{
    _id = id;
}

GraphNode::~GraphNode()
{
    //// STUDENT CODE TODO
    ////

    ////
    //// EOF STUDENT CODE
}

void GraphNode::AddToken(string token)
{
    _answers.push_back(token);
}

void GraphNode::AddEdgeToParentNode(GraphEdge *edge)
{
    _parentEdges.push_back(edge);
}

void GraphNode::AddEdgeToChildNode(unique_ptr<GraphEdge> edge) //task 4
{
    _childEdges.push_back(move(edge));
}

//// STUDENT CODE TODO
////
void GraphNode::MoveChatbotHere(ChatBot chatbot)
{
    _chatBot = move(chatbot);
    _chatBot.SetCurrentNode(this);
}

void GraphNode::MoveChatbotToNewNode(GraphNode *newNode)
{
    newNode->MoveChatbotHere(move(_chatBot));
}
////
//// EOF STUDENT CODE

GraphEdge *GraphNode::GetChildEdgeAtIndex(int index)
{
    //// STUDENT CODE TODO
    ////

    return _childEdges[index].get(); // task 4

    ////
    //// EOF STUDENT CODE
}