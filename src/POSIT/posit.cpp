#include "posit.h"
#include "operations.h"

using namespace std;

chrono::duration<double> total_time(0);

Posit::Posit(POSIT_UTYPE bits, int nbits, int es) :
    number(bits),
    number_size(nbits),
    exp_size(es)
{
    operations_count++;
    all_operations_count["="]++;
    start_time = chrono::high_resolution_clock::now();
}

Posit::Posit(int nbits, int es) :
    Posit(POSIT_ZERO, nbits, es)
{
}

bool Posit::isZero() const
{
    return util_is_zero(number);
}

bool Posit::isNar() const
{
    return util_is_nar(number);
}

bool Posit::isNeg() const
{
    return util_is_neg(number);
}

int Posit::nbits() const
{
    return number_size;
}

int Posit::ss() const
{
    return util_ss();
}

int Posit::rs() const
{
    return util_rs(number, number_size);
}

int Posit::es() const
{
    return util_es(number, number_size, exp_size);
}

int Posit::fs() const
{
    return util_fs(number, number_size, exp_size);
}

Posit Posit::zero() const
{
    return Posit(POSIT_ZERO, number_size, exp_size);
}

Posit Posit::one() const
{
    return Posit(POSIT_ONE, number_size, exp_size);
}

Posit Posit::nar() const
{
    return Posit(POSIT_NAR, number_size, exp_size);
}

Posit Posit::neg() const
{
    if (isNar()) {
        return nar();
    }

    return Posit(util_neg(number, number_size), number_size, exp_size);
}

Posit Posit::rec() const
{
    if (isNar() || isZero()) {
        return nar();
    }

    return one().div(*this);
}

std::string Posit::get_sign_bit()
{
    return toBinaryString().substr(0, ss());
}

std::string Posit::get_regime()
{
    return toBinaryString().substr(1, rs());
}

std::string Posit::get_exponent()
{
    return toBinaryString().substr(rs() + 1, es());
}

std::string Posit::get_fractional()
{
    return toBinaryString().substr(rs() + es() + 1, fs());
}

Posit Posit::add(const Posit& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero()) {
        return p;
    } else if (p.isZero()) {
        return *this;
    } else if (neg().eq(p)) {
        return zero();
    }

    unpacked_t aup = unpack_posit(number, number_size, exp_size);
    unpacked_t bup = unpack_posit(p.number, p.number_size, p.exp_size);
    unpacked_t up = op2_add(aup, bup);

    total_time += chrono::high_resolution_clock::now() - start_time;

    return Posit(pack_posit(up, number_size, exp_size), number_size, exp_size);
}

Posit Posit::sub(const Posit& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero()) {
        return p.neg();
    } else if (p.isZero()) {
        return *this;
    } else if (eq(p)) {
        return zero();
    }

    unpacked_t aup = unpack_posit(number, number_size, exp_size);
    unpacked_t bup = unpack_posit(p.number, p.number_size, p.exp_size);
    unpacked_t up = op2_sub(aup, bup);

    total_time += chrono::high_resolution_clock::now() - start_time;

    return Posit(pack_posit(up, number_size, exp_size), number_size, exp_size);
}

Posit Posit::mul(const Posit& p) const
{
    if (isNar() || p.isNar()) {
        return nar();
    } else if (isZero() || p.isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_posit(number, number_size, exp_size);
    unpacked_t bup = unpack_posit(p.number, p.number_size, p.exp_size);
    unpacked_t up = op2_mul(aup, bup);

    total_time += chrono::high_resolution_clock::now() - start_time;

    return Posit(pack_posit(up, number_size, exp_size), number_size, exp_size);
}

Posit Posit::div(const Posit& p) const
{
    if (isNar() || p.isNar() || p.isZero()) {
        return nar();
    } else if (isZero()) {
        return zero();
    }

    unpacked_t aup = unpack_posit(number, number_size, exp_size);
    unpacked_t bup = unpack_posit(p.number, p.number_size, p.exp_size);
    unpacked_t up = op2_div(aup, bup);

    total_time += chrono::high_resolution_clock::now() - start_time;

    return Posit(pack_posit(up, number_size, exp_size), number_size, exp_size);
}

bool Posit::eq(const Posit& p) const
{
    total_time += chrono::high_resolution_clock::now() - start_time;
    return number == p.number;
}

bool Posit::gt(const Posit& p) const
{
    total_time += chrono::high_resolution_clock::now() - start_time;
    return (POSIT_STYPE)number > (POSIT_STYPE)p.number;
}

bool Posit::ge(const Posit& p) const
{
    total_time += chrono::high_resolution_clock::now() - start_time;
    return gt(p) || eq(p);
}

bool Posit::lt(const Posit& p) const
{
    total_time += chrono::high_resolution_clock::now() - start_time;
    return !gt(p) && !eq(p);
}

bool Posit::le(const Posit& p) const
{
    total_time += chrono::high_resolution_clock::now() - start_time;
    return !gt(p);
}

void Posit::set(Posit p)
{
    number = pack_posit(unpack_posit(p.number, p.number_size, p.exp_size), number_size, exp_size);
}

void Posit::set(float n)
{
    switch (fpclassify(n)) {
    case FP_INFINITE:
    case FP_NAN:
        number = POSIT_NAR;
        break;
    case FP_ZERO:
        number = POSIT_ZERO;
        break;
    default:
        number = pack_posit(unpack_float(n), number_size, exp_size);
        break;
    }
}

void Posit::set(double n)
{
    switch (fpclassify(n)) {
    case FP_INFINITE:
    case FP_NAN:
        number = POSIT_NAR;
        break;
    case FP_ZERO:
        number = POSIT_ZERO;
        break;
    default:
        number = pack_posit(unpack_double(n), number_size, exp_size);
        break;
    }
}


float Posit::to_float() const
{
    if (isZero()) {
        return 0.f;
    } else if (isNar()) {
        return 0.f / 0.f;
    }

    return pack_float(unpack_posit(number, number_size, exp_size));
}

double Posit::to_double() const
{
    if (isZero()) {
        return 0.0;
    } else if (isNar()) {
        return 0.0 / 0.0;
    }

    return pack_double(unpack_posit(number, number_size, exp_size));
}

std::string Posit::toBinaryString() const 
{
        std::bitset<sizeof(POSIT_UTYPE) * 8> bits(number);
        return bits.to_string();
}

std::string Posit::positToBinary(const Posit& p) 
{
    return p.toBinaryString();
}


void Posit::setBits(POSIT_UTYPE bits)
{
    number = LSHIFT(bits, POSIT_WIDTH - number_size);
}

POSIT_UTYPE Posit::getBits()
{
    return RSHIFT(number, POSIT_WIDTH - number_size);
}

std::bitset<32> Posit::getBitset() const
{
    return std::bitset<32>(number);
}

void Posit::setBitset(std::bitset<32> bits)
{
    number = bits.to_ulong();
}

void Posit::setCountOperations(int c)
{
    operations_count = c;
}

double Posit::getTotalTime() 
{
    return total_time.count();
}

/*
  Prints double representation of posit number.
*/
void Posit::print()
{
    printf(" = %lg\n", to_double());
}

int Posit::getCountOperations()
{
    return operations_count;
}

std::map<std::string, int> Posit::getAllOpsCount()
{
    return all_operations_count;
}

/*
  Prints all bits of posit number.
  It prints out the format {number_size, exp_size}.
  Then it prints the binary representation of the posit's bits. 
  Afterwards, it prints out the sign of the posit (+ or -). 
  Next, it prints out the binary representation of posit, 
   placing a space after the exponent and regime bits.
*/
void Posit::printb()
{
    Posit p = (isNeg() ? neg() : *this);
    printf("{%d, %d} ", number_size, exp_size);

    if (isNar()) {
        printf("NaR\n");
        return;
    }

    printf("Bits: ");
    for (int i = POSIT_WIDTH - 1; i >= 0; i--) {
        printf("%d", RSHIFT(number, i) & 1);
    }
    printf("; Accurate binary representation: ");
    printf(isNeg() ? "-" : "+");

    for (int i = POSIT_WIDTH - 1; i >= 0; i--) {
        printf("%d", RSHIFT(p.number, i) & 1);

        if (i != 0 &&
            ((i == POSIT_WIDTH - ss() - p.rs()) ||
             (i == POSIT_WIDTH - ss() - p.rs() - exp_size))) {
            printf(" ");
        }
    }
    printf("\n");
}

std::ostream& operator<<(std::ostream& os, const Posit& p) 
{
    os << "{" << p.number_size << ", " << p.exp_size << "}, Decimal: " << p.number;
    os << "; Double representaion: " << p.to_double();
    os << "; Binary representaion: " << p.toBinaryString();
    return os;
}

std::istream& operator>>(std::istream& is, Posit& p) 
{
    int choice;
    std::cout << "Enter 1, 2 or 3 to choose which way you'd prefer to enter a POSIT number" << std::endl;
    std::cout << "1: Binary representation; 2: Float number; 3: Double number; Anything else: Double number" << std::endl;
    is >> choice;
    if (choice == 1) {
        POSIT_UTYPE bits;
        int nbits, es;
        std::cout << " Enter three numbers divided by space: \n  1.Decimal representation of POSIT number; \n  2.Number of bits in the posit format; \n  3.Number of exponent bits in the posit format" << std::endl;
        is >> bits >> nbits >> es;
        p = Posit(bits, nbits, es);
        return is;
    } else if (choice == 2) {
        float f;
        std::cout << "Enter float number:" << std::endl;
        is >> f;
        p = Posit(f);
        return is;
    }
    else if (choice == 3) {
        double d;
        std::cout << "Enter double number:" << std::endl;
        is >> d;
        p = Posit(d);
        return is;
    }
    else {
        double d;
        std::cout << "Enter double number:" << std::endl;
        is >> d;
        p = Posit(d);
        return is;
    }
}

Posit::Posit() :
    Posit(32, 2)
{
}

Posit::Posit(int v) :
    Posit()
{
    double d = (double)v;
    set(d);
}

Posit::Posit(float v) :
    Posit()
{
    set(v);
}

Posit::Posit(double v) :
    Posit()
{
    set(v);
}

Posit operator+(const Posit& a, const Posit& b)
{
    a.all_operations_count["+"]++;
    b.all_operations_count["+"]++;
    a.operations_count++;
    b.operations_count++;
    return a.add(b);
}

Posit operator-(const Posit& a, const Posit& b)
{
    a.all_operations_count["-"]++;
    b.all_operations_count["-"]++;
    a.operations_count++;
    b.operations_count++;
    return a.sub(b);
}

Posit operator*(const Posit& a, const Posit& b)
{
    a.all_operations_count["*"]++;
    b.all_operations_count["*"]++;
    a.operations_count++;
    b.operations_count++;
    return a.mul(b);
}

Posit operator/(const Posit& a, const Posit& b)
{
    a.all_operations_count["/"]++;
    b.all_operations_count["/"]++;
    a.operations_count++;
    b.operations_count++;
    return a.div(b);
}

Posit operator-(const Posit& a)
{
    a.all_operations_count["-"]++;
    a.operations_count++;
    return a.neg();
}

bool operator<(const Posit&a , const Posit& b)
{
    a.all_operations_count["<"]++;
    b.all_operations_count["<"]++;
    a.operations_count++;
    b.operations_count++;
    return a.lt(b);
}

bool operator<=(const Posit&a , const Posit& b)
{
    a.all_operations_count["<="]++;
    b.all_operations_count["<="]++;
    a.operations_count++;
    b.operations_count++;
    return a.le(b);
}

bool operator>(const Posit&a , const Posit& b)
{
    a.all_operations_count[">"]++;
    b.all_operations_count[">"]++;
    a.operations_count++;
    b.operations_count++;
    return a.gt(b);
}

bool operator>=(const Posit&a , const Posit& b)
{
    a.all_operations_count[">="]++;
    b.all_operations_count[">="]++;
    a.operations_count++;
    b.operations_count++;
    return a.ge(b);
}
bool operator==(const Posit&a , const Posit& b)
{
    a.all_operations_count["=="]++;
    b.all_operations_count["=="]++;
    a.operations_count++;
    b.operations_count++;
    return a.eq(b);
}

bool operator!=(const Posit&a , const Posit& b)
{
    a.all_operations_count["!="]++;
    b.all_operations_count["!="]++;
    a.operations_count++;
    b.operations_count++;
    return !a.eq(b);
}
