//
//
#pragma once
#include <exception>
#include <string>
#include <sstream>


class FormulaV1Exception :public std::exception
{
public:
	FormulaV1Exception(const std::string& message) : _message(message)
	{
	}

	virtual ~FormulaV1Exception() throw() 
	{ 
	}

	std::string message() const
	{
		return _message;
	}

private:
	std::string _message;
};

class FormulaV1 
{
public:
	FormulaV1() : _a_hasval(false), _b_hasval(false), _c_hasval(false), _d_hasval(false), _e_hasval(false)
	{
	}

	void reset()
	{
		_a_hasval = _b_hasval = _c_hasval = _d_hasval = _e_hasval = false;
	}

	void calculate() 
	{
		switch(getUnknownsKey())
		{
		case 3:		// ab
			setA(3.1);
			setB(3.2);
			break;
		case 5:		// ac
			setA(5.1);
			setC(5.2);
			break;
		case 9:		// ad
			setA(9.1);
			setD(9.2);
			break;
		case 17:	// ae
			setA(17.1);
			setE(17.2);
			break;
		case 6:		// bc
			setB(6.1);
			setC(6.2);
			break;
		case 10:	// bd
			setB(10.1);
			setD(10.2);
			break;
		case 18:	// be
			setB(18.1);
			setE(18.2);
			break;
		case 12:	// cd
			setC(12.1);
			setD(12.2);
			break;
		case 20:	// ce
			setC(20.1);
			setE(20.2);
			break;
		case 24:	// de
			setD(24.1);
			setE(24.2);
			break;
		default:
			throw FormulaV1Exception("Unrecognized combination of unknowns");
		}
	}

#pragma region get/set/has Var
void setA(double val)
{
	_a = val;
	_a_hasval = true;
}

double getA() const 
{
	if (hasA())
		return _a;
	throw FormulaV1Exception("Missing A");
}

bool hasA() const
{
	return _a_hasval;
}

	void setB(double val)
	{
		_b = val;
		_b_hasval = true;
	}

	double getB() const 
	{
		if (hasB())
			return _b;
		throw FormulaV1Exception("Missing B");
	}

	bool hasB() const
	{
		return _b_hasval;
	}

	void setC(double val)
	{
		_c = val;
		_c_hasval = true;
	}

	double getC() const 
	{
		if (hasC())
			return _c;
		throw FormulaV1Exception("Missing C");
	}

	bool hasC() const
	{
		return _c_hasval;
	}

	void setD(double val)
	{
		_d = val;
		_d_hasval = true;
	}

	double getD() const
	{
		if (hasD())
			return _d;
		throw FormulaV1Exception("Missing D");
	}

	bool hasD() const
	{
		return _d_hasval;
	}

	void setE(double val)
	{
		_e = val;
		_e_hasval = true;
	}

	double getE() const 
	{
		if (hasE())
			return _e;
		throw FormulaV1Exception("Missing E");
	}

	bool hasE() const
	{
		return _e_hasval;
	}

#pragma endregion

private:

	unsigned getUnknownsKey() const
	{
		return !hasA() + (!hasB() << 1) + (!hasC() << 2) + (!hasD() << 3) + (!hasE() << 4); 
	}

	double	_a, _b, _c, _d, _e;
	bool	_a_hasval, _b_hasval, _c_hasval, _d_hasval, _e_hasval;
};