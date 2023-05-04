#include "tests.h"

void posit_op_tests(){
     //POSIT
    std::cout << "POSIT OPERATIONS TESTING: " << std::endl;
    Posit a = Posit(12.5);
    Posit b = Posit(5.2);
    Posit add = a + b;
    Posit sub = a - b;
    Posit mul = b * a;
    Posit div = a / b;

    std::cout << "Addition testing";
    //Printing simple double representation test
    add.print();
    //Printing binary representation test
    add.printb();
    //Cout overload test
    std::cout << add << std::endl;

    std::cout << "Substraction testing";
    sub.print();
    sub.printb();

    std::cout << "Multiplication testing";
    mul.print();
    mul.printb();

    std::cout << "Division testing";
    div.print();
    div.printb();

    std::cout << "Commutative multiplication testing: " << std::endl;
    Posit mul1 = a * 2.5;
    mul1.print();
    mul1.printb();
    Posit mul2 = 2.5 * a;
    mul2.print();
    mul2.printb();
}

void posit_bool_ops_tests(){
    std::cout << "POSIT BOOL OPS TESTING: " << std::endl;
    Posit double_A(4.2);
    Posit double_B(8.6);
    Posit float_C(3.2f);
    Posit float_D(3.2f);
    bool gt = float_D > double_B;
    bool lt = double_A < double_B;
    bool neq = double_A != double_B;
    bool eq = float_C == float_D;
    bool geq = float_C >= float_D;
    bool leq = float_C <= double_B;
    std::cout << "GT testing: " << float_D.to_float() << " > " << double_B.to_float() << " = " << gt << std::endl;
    std::cout << "LT testing: " << double_A.to_float() << " < " << double_B.to_float() << " = " << lt << std::endl;
    std::cout << "NEQ testing: " << double_A.to_float() << " != " << double_B.to_float() << " = " << neq << std::endl;
    std::cout << "EQ testing: " << float_C.to_float() << " == " << float_D.to_float() << " = " << eq << std::endl;
    std::cout << "GEQ testing: " << float_C.to_float() << " >= " << float_D.to_float() << " = " << geq << std::endl;
    std::cout << "GEQ testing: " << float_C.to_float() << " <= " << double_B.to_float() << " = " << leq << std::endl;
}


void posit_cin_cout_test(){
    std::cout << "POSIT CIN & COUT TESTING: " << std::endl;
    Posit p(2, 0), p1(2, 0), p2(2, 0);
    std::cout << "Enter first posit: ";
    std::cin >> p;
    std::cout << "The posit you entered is: " << p << std::endl;
    std::cout << "Enter second posit: ";
    std::cin >> p1;
    std::cout << "The posit you entered is: " << p1 << std::endl;
    std::cout << "Enter third posit: ";
    std::cin >> p2;
    std::cout << "The posit you entered is: " << p2 << std::endl;
}

void posit_loop_test(){
    Posit a = Posit(2341234.1234);
    std::cout << " POSIT LOOP TESTING: " << std::endl;
    Posit loop_posit0 = Posit(0.0);
    std::cout << " POSIT loop test for addition: " << std::endl;
    for (int i = 0; i < 1000; i++)
    {
         loop_posit0 = loop_posit0 + a;
    }

    loop_posit0.print();
    loop_posit0.printb();
    std::cout << "Number of operations of a: " << a.getCountOperations() << std::endl;
    for (auto element : a.getAllOpsCount())
    {
         std::cout << "Number: a; " << "Operation: " << element.first << "; Used: " << element.second << std::endl;
    }
    std::cout << "Time of a: " << a.getTotalTime() << std::endl;
}