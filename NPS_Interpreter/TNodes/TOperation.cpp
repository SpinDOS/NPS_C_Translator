#include "TNode.h"
#include "../Operations/OperationManager.h"

using namespace NPS_Interpreter;

char* TOperation::Exec()
{
    char *a;
    switch (method)
    {
        case BinaryPlusDoubles:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::BinaryPlusDoublesM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case BinaryMinusDoubles:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::BinaryMinusDoublesM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case MultiplyDoubles:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::MultiplyDoublesM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case DivideDoubles:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::DivideDoublesM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case PrefixIncDouble:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::PrefixIncDoubleM(children.get(0)->Exec());
            break;
        case PrefixDecDouble:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::PrefixDecDoubleM(children.get(0)->Exec());
            break;
        case PostfixIncDouble:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::PostfixIncDoubleM(children.get(0)->Exec());
            break;
        case PostfixDecDouble:
            a = (char *) Heap::get_mem(sizeof(double));
            *(double*)a = OperationManager::PostfixDecDoubleM(children.get(0)->Exec());
            break;
        case CmpEqualDoubles:
            a = (char *) Heap::get_mem(sizeof(bool));
            *(bool*)a = OperationManager::EqualsDoubleM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case CmpNotEqualDoubles:
            a = (char *) Heap::get_mem(sizeof(bool));
            *(bool*)a = !OperationManager::EqualsDoubleM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case CmpMoreDoubles:
            a = (char *) Heap::get_mem(sizeof(bool));
            *(bool*)a = OperationManager::CmpMoreDoublesM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case CmpLessDoubles:
            a = (char *) Heap::get_mem(sizeof(bool));
            *(bool*)a = OperationManager::CmpLessDoublesM(children.get(0)->Exec(), children.get(1)->Exec());
            break;
        case Assignment:
            return OperationManager::AssignmentM(children.get(0)->Exec(), children.get(1)->Exec(), size);
        case Comma:
            children.get(0)->Exec();
            children.get(1)->Exec();
            break;
    }
    return a;
}
