// File: main.cpp
//
#include <iostream>
#include "FormulaV1.h"
#include "FormulaV2.h"
#include "FormulaV3.h"
#include "FormulaV4a.h"
#include "FormulaV4b.h"

int main(int argc, char **argv)
{
	FormulaV1 formulaV1;
	formulaV1.setA(23.33); 
	formulaV1.setC(1.23); 
	formulaV1.setE(0.84755);
	formulaV1.calculate();
	std::cout << "v1   => a=" << formulaV1.getA() << ", b=" << formulaV1.getB() << ", c=" << formulaV1.getC() << ", d=" << formulaV1.getD() << ", e=" << formulaV1.getE() << std::endl;

	FormulaV2 formulaV2;
	formulaV2.setA(23.33); 
	formulaV2.setC(1.23); 
	formulaV2.setE(0.84755);
	formulaV2.calculate();
	std::cout << "V2   => a=" << formulaV2.getA() << ", b=" << formulaV2.getB() << ", c=" << formulaV2.getC() << ", d=" << formulaV2.getD() << ", e=" << formulaV2.getE() << std::endl;

	FormulaV3 formulaV3;
    formulaV3.set(FormulaV3::tag::b, 1.4);
    formulaV3.set(FormulaV3::tag::c, 1.0);
    formulaV3.set(FormulaV3::tag::d, 2.0);
    formulaV3.calculate();
    std::cout << "v3    => a= " << formulaV3.get(FormulaV3::tag::a) << ", b= " << formulaV3.get(FormulaV3::tag::b) << ", c= " << formulaV3.get(FormulaV3::tag::c) << ", d= " << formulaV3.get(FormulaV3::tag::d) << ", e= " << formulaV3.get(FormulaV3::tag::e) << std::endl;

	FormulaV4a formulaV4a;
	formulaV4a.set(ValueId::acceleration, 1.3);
	formulaV4a.set(ValueId::final_velocity, 8.7);
	formulaV4a.set(ValueId::time, 38.351);
	formulaV4a.calculate();
	std::cout << "v4a   => acc=" << formulaV4a.get(ValueId::acceleration) << ", dist=" << formulaV4a.get(ValueId::distance) << ", vinitial=" << formulaV4a.get(ValueId::initial_velocity) << ", vfinal=" << formulaV4a.get(ValueId::final_velocity) << ", time="<< formulaV4a.get(ValueId::time) << ", " << std::endl;

	FormulaV4b formulaV4b;
	typedef FormulaV4b::tag tv4b; // shortcut
	formulaV4b.set(tv4b::acceleration, 1.3);
	formulaV4b.set(tv4b::final_velocity, 8.7);
	formulaV4b.set(tv4b::time, 38.351);
	formulaV4b.calculate();
	std::cout << "v4b   => acc=" << formulaV4b.get(tv4b::acceleration) << ", dist=" << formulaV4b.get(tv4b::distance) << ", vinitial=" << formulaV4b.get(tv4b::initial_velocity) << ", vfinal=" << formulaV4b.get(tv4b::final_velocity) << ", time="<< formulaV4b.get(tv4b::time) << ", " << std::endl;

	return 0;
}