#include <iostream>
#include <fstream>
#include "LexemeParsing/LexemeParser.h"
#include "ErrorReporter/ErrorReporter.h"
#include "Sentences/SentenceParser.h"
#include "TNodes/TNode.h"
#include "../NPS_library/collection_containers/TSimpleLinkedList.h"
#include "TNodes/ResultType.h"
#include "../NPS_library/collection_containers/THashTable.h"
#include "Operations/PrimitiveOperationsManager.h"

using namespace std;

void getLinePosOfChar(const char *start, unsigned long position, int &line, int &pos);

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
    SentenceParser sentenceParser(&words);
    TList *list = sentenceParser.ParseList();
    if (ErrorReported())
    {
        int line, pos;
        Error *error = GetError();
        char *invalidWord = copy_string(contents.c_str() +
                error->error_pos - min(error->error_pos, 5ul), 6 + min(error->error_pos, 5ul));
        getLinePosOfChar(contents.c_str(), error->error_pos, line, pos);
        cout << "Error: '" << error->message << "' in the lexeme \"" << invalidWord
             << "\" (line: " << line << ", position: " << pos << ")" << endl;
        Heap::free_mem(invalidWord);
        return 1;
    }
    for (int i = 0; i < list->children.count() - 1; i++)
    {
        list->children.get(i)->Print(0);
        cout << "Parse next sentence? ";
        string a;
        cin >> a;
    }
    if (list->children.count() > 0)
        list->children.getLast()->Print(0);
    return 0;
}

void getLinePosOfChar(const char *start, unsigned long position, int &line, int &pos)
{
    line = 1;
    pos = 1;
    for (unsigned long i = 0; i < position; i++)
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