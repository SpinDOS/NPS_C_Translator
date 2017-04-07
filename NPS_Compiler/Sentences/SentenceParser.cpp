//
// Created by Alexander on 25-Mar-17.
//

#include "SentenceParser.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Types/TypesManager.h"

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

bool SentenceParser::GetNextSentence(TSimpleLinkedList<LexemeWord*> &sentence) // int a = foo(b,c);
{
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
    return false;
}


TNode *SentenceParser::ParseNextSentence()
{
    TBranch *root = new TList;
    TBranch *cur = root;
    bool hasleft = false;
    bool expectedRight = true;
    TDeclaration *lastDeclaration = nullptr;
    const char *lastType;
    for (int curPos = 0; curPos < text->count(); curPos++)
    {
        if (ErrorReported())
            return nullptr;
        LexemeWord *word = text->getTyped(curPos);
        if (100 <= word->code && word->code < 200) // constant
        {
            cur->children.add(GetTLeaf(word, hasleft, expectedRight));
        }
        else if (300 <= word->code && word->code < 400) // keywords - if, while, for...
        {
            // will be included in lab 4
        }
        else if (400 <= word->code && word->code < 600)
        {
            if (TypesManager::GetTypeInfo(*word) != nullptr) // type declaration
            {
                if (lastDeclaration != nullptr)
                {
                    ReportError(word->start, "Unexpected type declaration");
                    break;
                }
                lastDeclaration = new TDeclaration;
                lastDeclaration->type = new ResultType(*word);
                lastType = copy_string(word->start, word->length);
            }
            //else if FunctionsManager::GetFunctionInfo(*word) != nullprt) ...
            else // variable
            {
                if (lastDeclaration == nullptr) // just variable use
                {
                    cur->children.add(GetTLeaf(word, hasleft, expectedRight));
                    continue;
                }
                lastDeclaration->var = copy_string(word->start, word->length);
                // do something with declaration
            }
        }


    }
    if (ErrorReported())
        return nullptr;
    return root;
}

