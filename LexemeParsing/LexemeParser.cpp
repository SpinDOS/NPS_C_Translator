//
// Created by Alexander on 07-Mar-17.
//

#include "LexemeParser.h"

Queue<LexemeInfo> ParseToLexemes(const char *s, TypeList<LexemeParsingException> &errors)
{
    // парсер лексемы должен вернуть nullptr, если это не его лексема
    // выборосить LexemeParsingException, если его лексема, но в ней ошибка
    // вернуть LexemeInfo, если все ок, где description - указатель на определенный вами тип, описывающий лексему
    // в любом случае, он должен установить offset на длину своей лексемы (с ошибкой или без)

    Queue<LexemeInfo> lexemes;
    LexemeInfo *lexemeInfo;
    int offset = 0;
    const char *start = s;
    while (*s)
    {
        try {
            //if (lexemInfo = ConstantLexemParser::Parse(s, offset) ||
              //  lexemInfo = VariableKeywordParser::Parse(s, offset) ||
                //lexemInfo = OperationParser::Parse(s, offset))
            {
//                lexemes.put(*lexemInfo);
  //              Heap::free_mem(lexemInfo);
            }
            //else
              //  throw
        }
        catch (LexemeParsingException e){

        }
        s += offset;
    }
    return lexemes;
}

//void get_line_and_pos_of_global_position(const char *global_position, const )