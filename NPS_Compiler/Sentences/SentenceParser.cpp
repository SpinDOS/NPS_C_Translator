//
// Created by Alexander on 25-Mar-17.
//

#include <iostream>
#include "SentenceParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"
#include "../Operations/CustomOperationsManager.h"

void HandleDeclaration(TBranch *parent, LexemeWord *type)
{
//    if (parent->lexeme->code != -1)
//    {
//        ReportError(first->start, "Variable declaration can not be placed here");
//        return;
//    }
//    LexemeWord *cur = text->get(curPos++);
//    int p_count = 0;
//    while (cur->code == 218) // *
//    {
//        cur = text->get(curPos++);
//        p_count++;
//    }
//    if (cur->code < 400 || cur->code >= 600) // not variable name
//    {
//        ReportError(cur->start, "Invalid variable declaration");
//        return;
//    }
//    if ()
//
////    TDeclaration *result = new TDeclaration;
////    result->lexeme = text.get(curPos++);
////    result->parent = parent;
////    result->type = new ResultType;
////    result->type->


}

//bool SentenceParser::GetNextSentence(TSimpleLinkedList<LexemeWord*> &sentence) // int a = foo(b,c);
//{
    /*TList root;
    root.lexeme = new LexemeWord;
    root.lexeme->code = -1;
    TBranch *current = &root;
    for (int curPos = 0; !ErrorReported() && curPos < text->count(); curPos++)
    {
        LexemeWord *lexemeWord = text->getTyped(curPos);
        if (TypesManager::GetTypeInfo(*lexemeWord))
        {
            if (lastType != nullptr)
            {
                ReportError(lexemeWord->start, "Unexpected type declaration");
                return false;
            }
            lastType = lexemeWord;
        }



    }*/
//    int max = text->count();
//    if (curPos >= max)
//        return false;
//    LexemeWord *word;
//    if (lastType != nullptr)
//        sentence.add(lastType);
//    else
//    {
//        word = text->getTyped(curPos++);
//        sentence.add(word);
//        if (TypesManager::GetTypeInfo(*word))
//            lastType = word;
//    }
//    while (curPos < max)
//    {
//        word = text->getTyped(curPos++);
//        sentence.add(word);
//        if (word->code == 243) // ;
//        {
//            lastType = nullptr;
//            return true;
//        }
//        else if (word->code == 242 && lastType != nullptr) // ,
//        {
//            return true;
//        }
//    }
//    ReportError(word->start + word->length, "Unexpected end of file");
   //return false;
//}

struct TRoot : public TBranch
{
public:
    TRoot()
    {Priority = -1000000;}
    ResultType* _getType() final{return 0;}
};

TNode *HandleVarDeclaration();

TNode *SentenceParser::HandleExpression()
{
    TRoot *root = new TRoot;
    TBranch *cur = root;
    bool hasLeft = false, expectedRight = true;
    while (!ErrorReported())
    {
        LexemeWord *word = text->getTyped(curPos++);
        if (100 <= word->code && word->code < 200) // constant
        {
            TNode *node = GetTLeaf(word, hasLeft, expectedRight);
            node->parent = cur;
            cur->children.add(node);
            continue;
        }
        if (300 <= word->code && word->code < 400)// keyword
        {
            ReportError(word->start, "Unexpected keyword");
            break;
        }
        if (400 <= word->code && word->code < 600)// varname
        {
            if (TypesManager::GetTypeInfo(*word)) // type declaration
            {
                ReportError(word->start, "Unexpected type declaration");
                break;
            }
            if (CustomOperationsManager::IsOperationExists(*word)) // function
            {
            
            }
            TNode *node = GetTLeaf(word, hasLeft, expectedRight); // variable
            node->parent = cur;
            cur->children.add(node);
            continue;
        }
        // operation
        TBranch *branch = GetTBranch(word, hasLeft, expectedRight);
        if (ErrorReported())
            return nullptr;
    
        if (word->code == 204 || word->code == 206) // ( [
        {
            branch->parent = cur;
            cur->children.add(branch);
            cur = branch;
            continue;
        }
    
        while (branch->Priority > cur->Priority
               || (branch->Priority == cur->Priority && cur->IsLeftAssociated))
        {
            if ((branch->lexeme->code == 205 && cur->lexeme->code == 204) // ()
                || (branch->lexeme->code == 206 && cur->lexeme->code == 207) // []
                || (branch->lexeme->code == 239 && cur->lexeme->code == 240)) // ?:
                break;
            cur = cur->parent;
        }
    
        if (cur == root && !expectedRight) // ; ) ]
        {
            curPos--;
            if (cur->children.count() > 1)
                std::cout << "ALARM! ROOT HAS MULTIPLE CHILDREN";
            TNode *node = nullptr;
            if (cur->children.count() != 0)
            {
                node = cur->children.takeLast();
                node->parent = nullptr;
            }
            delete root;
            delete branch;
            return node;
        }
    
        TBranch *parent = cur->parent;
        
        // handle () [] ?:
//        if (cur->lexeme->code == 204) // ()
//        {
//
//
//            if (cur->children.count() == 0)
//            {
//                if (parent->tNodeType != TNodeType::TFunction)
//                {
//                    ReportError(branch->lexeme->start, "Expression expected");
//                    return nullptr;
//                }
//                parent->children.clear();
//                delete branch;
//                delete cur;
//                cur = parent;
//                continue;
//            }
//            TNode *child;
//
//            cur->children.getFirst()->parent = parent;
//
//        }
        
        branch->parent = cur;
        branch->children.add(cur->children.takeLast());
        branch->children.getLast()->parent = branch;
        cur = branch;
    }
    return nullptr;
}

TNode *SentenceParser::ParseNextSentence()
{
//    TBranch *root = new TList;
//    TBranch *cur = root;
//    for (int curPos = 0; !ErrorReported() && curPos < text->count(); curPos++) // loop for sentences
//    {
//        LexemeWord *typeOfDeclaration = text->getTyped(curPos);
//        if (!TypesManager::GetTypeInfo(*typeOfDeclaration) != nullptr) // type declaration
//            typeOfDeclaration = nullptr;
//        lastDeclaration = nullptr;
//        if (typeOfDeclaration)
//        {
//            lastDeclaration = new TDeclaration;
//            lastDeclaration->type = new ResultType(*typeOfDeclaration, 0, false);
//            curPos++;
//        }
//        hasLeft = false;
//        expectedRight = true;
//        do
//        {
//
//        }
//        while (!ErrorReported() && cur != root);// loop for lexemes in the sentence
//    }
//    if (ErrorReported())
//        return nullptr;
//    return root;
    return 0;
}

