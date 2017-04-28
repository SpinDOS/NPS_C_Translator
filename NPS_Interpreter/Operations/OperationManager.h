#ifndef NPS_INTERPRETER_OPERATIONMANAGER_H
#define NPS_INTERPRETER_OPERATIONMANAGER_H

#include <cstring>

class OperationManager {

public:

    static char* Min(char* a, char* b){
        return *(double*)a < *(double*)b ? a : b;
    }

    static char* Max(char* a, char* b){
        return *(double*)a > *(double*)b ? a : b;
    }

    static void Output(char *a){
        while (*a)
            std::cout << *a++;
        std::cout << std::endl;
    }

    static void OutputDouble(char *a){
        double res = *(double*)a;
        std::cout << "Double " << res << std::endl;
    }

    static double BinaryPlusDoublesM(char* a, char* b){
        return *(double*)a + *(double*)b;
    }

    static double BinaryMinusDoublesM(char* a, char* b){
        return *(double*)a - *(double*)b;
    }
    static double MultiplyDoublesM(char* a, char* b){
        return *(double*)a * *(double*)b;
    }
    static double DivideDoublesM(char* a, char *b){
        return *(double*)a / *(double*)b;
    }
    static double PrefixIncDoubleM(char *a){
        return ++(*(double*)a);
    }

    static double PrefixDecDoubleM(char *a){
        return --(*(double*)a);
    }

    static double PostfixIncDoubleM(char *a){
        return (*(double*)a)++;
    }

    static double PostfixDecDoubleM(char *a){
        return (*(double*)a)++;
    }

    static char* AssignmentM(char *a, char *b, int size){
        memcpy(a, b, size);
        return a;
    }

    static bool EqualsDoubleM(char *a, char *b){
        return *(double*)a == *(double*)b;
    }

    static bool CmpMoreDoublesM(char *a, char *b){
        return *(double*)a > *(double*)b;
    }

    static bool CmpLessDoublesM(char *a, char *b){
        return *(double*)a < *(double*)b;
    }

};

#endif //NPS_INTERPRETER_OPERATIONMANAGER_H
