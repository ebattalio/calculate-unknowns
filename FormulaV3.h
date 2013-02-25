#include <array>
#include <bitset>
#include <functional>
#include <stdexcept>
#include <string>
#include <map>

struct FormulaV3Exception : std::runtime_error
{
	FormulaV3Exception(const std::string& err = "Something went wrong") 
		: std::runtime_error(err)
	{
	}
};

class FormulaV3
{
public:
	// a-e are variables, count is the number of variables
	enum struct tag : unsigned { a, b, c, d, e, count };

	FormulaV3()
		: _functions(create_function_map(this))
	{
	}

	void calculate()
	{
		// Find the right equation given the missing values (getUnknownsKey)
		auto const function_it(_functions.find(getUnknownsKey()));
		if (function_it == _functions.end())
			throw  FormulaV3Exception("Unrecognized combination of unknowns");
		// Compute the missing values
		function_it->second();
	}

	void set(tag const t, double const v)
	{
		_values[as_underlying(t)] = v;
		_presence[as_underlying(t)] = true;
	}

	bool has(tag const t) const
	{
		return _presence[as_underlying(t)];
	}

	double get(tag const t) const
	{
		return has(t) ? _values[as_underlying(t)] : throw FormulaV3Exception();
	}

	unsigned getUnknownsKey() const
	{
		return ~static_cast<unsigned>(_presence.to_ulong()) & (as_bit(tag::count) - 1);
	}

private:

	typedef std::array<double, static_cast<unsigned>(tag::count)> value_sequence;
	typedef std::bitset<static_cast<unsigned>(tag::count)>        presence_sequence;
	typedef std::map<unsigned, std::function<void()>>   function_map;

	static unsigned as_underlying(tag const t) { return static_cast<unsigned>(t); }
	static unsigned as_bit       (tag const t) { return 1 << as_underlying(t);    }

	static function_map create_function_map(FormulaV3* p)
	{
		function_map f;
		f[as_bit(tag::a) | as_bit(tag::b)] = std::bind(&FormulaV3::compute_ab, p);
		f[as_bit(tag::a) | as_bit(tag::c)] = std::bind(&FormulaV3::compute_ac, p);
		f[as_bit(tag::a) | as_bit(tag::d)] = std::bind(&FormulaV3::compute_ad, p);
		f[as_bit(tag::a) | as_bit(tag::e)] = std::bind(&FormulaV3::compute_ae, p);

		f[as_bit(tag::b) | as_bit(tag::c)] = std::bind(&FormulaV3::compute_bc, p);
		f[as_bit(tag::b) | as_bit(tag::d)] = std::bind(&FormulaV3::compute_bd, p);
		f[as_bit(tag::b) | as_bit(tag::e)] = std::bind(&FormulaV3::compute_be, p);

		f[as_bit(tag::c) | as_bit(tag::d)] = std::bind(&FormulaV3::compute_cd, p);
		f[as_bit(tag::c) | as_bit(tag::e)] = std::bind(&FormulaV3::compute_ce, p);

		f[as_bit(tag::d) | as_bit(tag::e)] = std::bind(&FormulaV3::compute_de, p);

		return f;
	}

	void compute_ab() { set(tag::a,  1.1); set(tag::b,  1.2); }
	void compute_ac() { set(tag::a,  2.1); set(tag::c,  2.2); }
	void compute_ad() { set(tag::a,  3.1); set(tag::d,  3.2); }
	void compute_ae() { set(tag::a,  4.1); set(tag::e,  4.2); }
	void compute_bc() { set(tag::b,  5.1); set(tag::c,  5.2); }
	void compute_bd() { set(tag::b,  6.1); set(tag::d,  6.2); }
	void compute_be() { set(tag::b,  7.1); set(tag::e,  7.2); }
	void compute_cd() { set(tag::c,  8.1); set(tag::d,  8.2); }
	void compute_ce() { set(tag::c,  9.1); set(tag::e,  9.2); }
	void compute_de() { set(tag::d, 10.1); set(tag::e, 10.2); }
	
	value_sequence    _values;
	presence_sequence _presence;
	function_map      _functions;
};
