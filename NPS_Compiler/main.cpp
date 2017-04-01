#include <iostream>
#include <fstream>
#include "LexemeParsing/LexemeParser.h"
#include "ErrorReporter/ErrorReporter.h"
#include "SentenceParser/SentenceParser.h"
#include "TNodes/TNode.h"
#include "../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "TNodes/ResultType.h"

using namespace std;

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos);

int main(int argc, char *argv[])
{
    ifstream ini_file("LexemeParsingInstructions.ini");
    if (!ini_file.good())
    {
        cout << "LexemeParsingInstructions.ini not found" << endl;
        return EXIT_FAILURE;
    }
    string instructions((std::istreambuf_iterator<char>(ini_file)),
                         std::istreambuf_iterator<char>());
    ini_file.close();
    
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <file to parse>" << endl;
        return EXIT_FAILURE;
    }
    ifstream file(argv[1]);
    if (!file.good())
    {
        cout << "Can not open file " << argv[1] << endl;
        return EXIT_FAILURE;
    }
    string contents((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    TypeList<LexemeWord> words;
    LexemeParser parser(instructions.c_str());
    parser.ParseToLexemes(contents.c_str(), words);
    if (ErrorReported())
    {
        int line, pos;
        Error *error = GetError();
        char *invalidWord = copy_string(error->error_pos, 10);
        getLinePosOfChar(contents.c_str(), error->error_pos, line, pos);
        cout << "Error: '" << error->message << "' in the lexeme \"" << invalidWord
             << "\" (line: " << line << ", position: " << pos << ")" << endl;
        Heap::free_mem(invalidWord);
        return 1;
    }


    /*unsigned int start = 0, end = 0;
    TNode **temp = (TNode**)Heap::get_mem(sizeof(TNode*));
    while (GetNextSentence(words, start, end))
    {
        double *d;
        TOperation op;
        op.children.add(new TConstant);
        op.priority = -1000;
        TNode *cur = &op;
        for (int i = start; i < end; i++)
        {
            TNode *newNode = GetTNode(words.getTyped(i));
            while (newNode->priority <= cur->priority)
                cur = cur->parent;
            if (cur->children.count() > 1)
            {
                cur->children.take_last(temp);
                newNode->children.add(temp);
                (*temp)->parent = newNode;
            }
            cur->children.add(&newNode);
            newNode->parent = cur;
            cur = newNode;
        }
        d = (double*)(*op.children.getTyped(1))->Exec();
        cout << *d << "\n";
        start = end+1;
    }*/
    return 0;
}

void getLinePosOfChar(const char *start, const char *position, int &line, int &pos)
{
    line = 1;
    pos = 1;
    while (start < position)
    {
        if (*start++ == '\n')
        {
            line++;
            pos = 1;
        }
        else
            pos++;
    }
}