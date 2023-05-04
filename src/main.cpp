
#include "TESTS//tests.h"

int main()
{
    std::cout << "\n";
    posit_cin_cout_test();
    std::cout << "\n";
    posit_op_tests();
    std::cout << "\n";
    posit_bool_ops_tests();
    std::cout << "\n";
    posit_loop_test();



    Posit number = Posit(123.456);
    std::cout << "number"; number.print();
    std::cout << number.get_sign_bit() << " " << number.get_regime() << " " <<
                 number.get_exponent() << " " << number.get_fractional() << std::endl;
    Posit add = number + number;
    std::cout << "number + number"; add.print();
    Posit mul = number * number;
    std::cout << "number * number"; mul.print();


    return 0;
}
