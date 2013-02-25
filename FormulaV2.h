//
//
#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <array>
#include <unordered_map>
#include <map>
#include <functional>

class FormulaV2Exception :public std::exception
{
public:
	FormulaV2Exception(const std::string& message) : _message(message)
	{
	}

	virtual ~FormulaV2Exception() throw() 
	{ 
	}

	std::string message() const
	{
		return _message;
	}

private:
	std::string _message;
};

class FormulaV2 
{
public:
	FormulaV2() : _a_hasval(false), _b_hasval(false), _c_hasval(false), _d_hasval(false), _e_hasval(false),_formulas(create_function_map(this))
	{
	}

	void Reset()
	{
		_a_hasval = _b_hasval = _c_hasval = _d_hasval = _e_hasval = false;
	}

	void calculate() 
	{
		// Find the right equation given the missing values (getUnknownsKey)
		auto const function_it(_formulas.find(getUnknownsKey()));
		if (function_it == _formulas.end())
			throw FormulaV2Exception("Unrecognized combination of unknowns");
		// Compute the missing values
		function_it->second();
	}


#pragma region get/Set/has Var
	void setA(double val)
	{
		_a = val;
		_a_hasval = true;
	}

	double getA() const 
	{
		if (hasA())
			return _a;
		throw FormulaV2Exception("Missing A");
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
		throw FormulaV2Exception("Missing B");
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
		throw FormulaV2Exception("Missing C");
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
		throw FormulaV2Exception("Missing D");
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
		throw FormulaV2Exception("Missing E");
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

	typedef std::map<unsigned int, std::function<void()>> FormulasV2;

	FormulasV2 _formulas;
	double	_a, _b, _c, _d, _e;
	bool	_a_hasval, _b_hasval, _c_hasval, _d_hasval, _e_hasval;

	#pragma region Functions to calculate compute_

	void compute_ab()
	{
		setA(3.1);
		setB(3.2);
	}

	void compute_ac()
	{
		setA(5.1);
		setC(5.2);
	}

	void compute_ad()
	{
		setA(9.1);
		setD(9.2);
	}

	void compute_ae()
	{
		setA(17.1);
		setE(17.2);
	}

	void compute_bc()
	{
		setB(6.1);
		setC(6.2);
	}

	void compute_bd()
	{
		setB(10.1);
		setD(10.2);
	}

	void compute_be()
	{
		setB(18.1);
		setE(18.2);
	}

	void compute_cd()
	{
		setC(12.1);
		setD(12.2);
	}

	void compute_ce()
	{
		setC(20.1);
		setE(20.2);
	}

	void compute_de()
	{
		setD(24.1);
		setE(24.2);
	}
#pragma endregion

	static FormulasV2 create_function_map(FormulaV2* p)
	{
		FormulasV2 formulas;
		formulas[3] = std::bind(&FormulaV2::compute_ab, p);
		formulas[5] = std::bind(&FormulaV2::compute_ac, p);
		formulas[9] = std::bind(&FormulaV2::compute_ad, p);
		formulas[17] = std::bind(&FormulaV2::compute_ae, p);
		formulas[6] = std::bind(&FormulaV2::compute_bc, p);
		formulas[10] = std::bind(&FormulaV2::compute_bd, p);
		formulas[18] = std::bind(&FormulaV2::compute_be, p);
		formulas[12] = std::bind(&FormulaV2::compute_cd, p);
		formulas[20] = std::bind(&FormulaV2::compute_ce, p);
		formulas[24] = std::bind(&FormulaV2::compute_de, p);
		return formulas;
	}
};

