// FormulaV4a.h
//
#include <array>
#include <bitset>
#include <functional>
#include <stdexcept>
#include <string>
#include <map>

struct FormulaV4bException : std::runtime_error
{
	FormulaV4bException(const std::string& err = "Something went wrong") 
		: std::runtime_error(err)
	{
	}
};

class FormulaV4b
{
public:
	// equation variables, count is the number of variables
	enum struct tag : unsigned {distance, time, initial_velocity, final_velocity, acceleration, count };

	FormulaV4b()
		: _functions(create_function_map(this))
	{
	}

	// calculate for two unknowns
	void calculate()
	{
		// Find the right equation given the missing values (getUnknownsKey)
		auto const function_it(_functions.find(getUnknownsKey()));
		if (function_it == _functions.end())
			throw FormulaV4bException("Unrecognized combination of unknowns");
		// Compute the missing values
		function_it->second();
	}

	// reset bits to false = no eqn vars have been set
	void reset()
	{
		_presence.reset();
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
		return has(t) ? _values[as_underlying(t)] : throw FormulaV4bException();
	}

	// returns state of unset vars using enums as bit positions
	unsigned getUnknownsKey() const
	{
		// ~ because bits are on for vars that have been set (for example, "01011"), so 
		// we need to bitwise not to reverse them (~"01011" = "10100", so distance and
		// initial_velocity are unset) 
		// & part masks off unused positions
		return ~static_cast<unsigned>(_presence.to_ulong()) & (as_bit(tag::count) - 1);
	}

private:
	typedef std::array<double, static_cast<unsigned>(tag::count)> value_sequence;
	typedef std::bitset<static_cast<unsigned>(tag::count)> presence_sequence;
	typedef std::map<unsigned, std::function<void()>> function_map;

	// enum element as unsigned 
	static unsigned as_underlying(tag const t) { return static_cast<unsigned>(t); }
	// bit value as unsigned
	static unsigned as_bit       (tag const t) { return 1 << as_underlying(t);    }

	// loads up our map of unknowns -> compute function that solves for unknowns
static function_map create_function_map(FormulaV4b* p)
{
	// note that the compute functions could be made lamdas
	// left this way, it is easy to see what is going on
	function_map f;
	f[as_bit(tag::distance) | as_bit(tag::time)] = std::bind(&FormulaV4b::compute_distance_time, p);
	f[as_bit(tag::distance) | as_bit(tag::initial_velocity)] = std::bind(&FormulaV4b::compute_distance_initial_velocity, p);
	f[as_bit(tag::distance) | as_bit(tag::final_velocity)] = std::bind(&FormulaV4b::compute_distance_final_velocity, p);
	f[as_bit(tag::distance) | as_bit(tag::acceleration)] = std::bind(&FormulaV4b::compute_distance_acceleration, p);

	f[as_bit(tag::time) | as_bit(tag::initial_velocity)] = std::bind(&FormulaV4b::compute_time_initial_velocity, p);
	f[as_bit(tag::time) | as_bit(tag::final_velocity)] = std::bind(&FormulaV4b::compute_time_final_velocity, p);
	f[as_bit(tag::time) | as_bit(tag::acceleration)] = std::bind(&FormulaV4b::compute_time_acceleration, p);

	f[as_bit(tag::initial_velocity) | as_bit(tag::final_velocity)] = std::bind(&FormulaV4b::compute_initial_velocity_final_velocity, p);
	f[as_bit(tag::initial_velocity) | as_bit(tag::acceleration)] = std::bind(&FormulaV4b::compute_initial_velocity_acceleration, p);

	f[as_bit(tag::final_velocity) | as_bit(tag::acceleration)] = std::bind(&FormulaV4b::compute_final_velocity_acceleration, p);
	return f;
}

	// compute helper functions
	double get_distance_using_time_initvel_acc() const 
	{
		// distance = initial_velocity * time + 0.5 * (acceleration * time^2)
		return get(tag::initial_velocity) * get(tag::time) + (0.5 * get(tag::acceleration)) * pow(get(tag::time), 2);
	}

	double get_time_using_vel_acc_or_dist() const
	{
		// if we have acceleration, compute time with it as denominator
		if (get(tag::acceleration) != 0)
			return (get(tag::final_velocity) - get(tag::initial_velocity)) / get(tag::acceleration);
		if (get(tag::final_velocity) != 0)
			return get(tag::distance) / get(tag::final_velocity);
		throw FormulaV4bException("Acceleration and final velocity cannot both be zero");
	}

	// compute functions
	void compute_distance_time() 
	{ 
		if (get(tag::acceleration) == 0)
			throw FormulaV4bException("Accleration cannot be zero when distance and time are unknowns");
		set(tag::time, (get(tag::final_velocity) - get(tag::initial_velocity)) / get(tag::acceleration));
		set(tag::distance, get_distance_using_time_initvel_acc());
	}

	void compute_distance_initial_velocity() 
	{ 
		set(tag::initial_velocity, get(tag::final_velocity) - get(tag::acceleration) * get(tag::time));
		set(tag::distance, get_distance_using_time_initvel_acc());
	}

	void compute_distance_final_velocity() 
	{ 
		set(tag::final_velocity, get(tag::initial_velocity) + get(tag::acceleration) * get(tag::time));
		set(tag::distance, get_distance_using_time_initvel_acc());
	}

	void compute_distance_acceleration() 
	{ 
		if (get(tag::time) == 0)
			throw FormulaV4bException("Time cannot be zero when distance and acceleration are unknown");
		set(tag::time, (get(tag::final_velocity) - get(tag::initial_velocity)) / get(tag::time));
		set(tag::distance, get_distance_using_time_initvel_acc());
	}

	void compute_time_initial_velocity() 
	{
		const double val = get(tag::final_velocity) - 2 * (get(tag::acceleration) * get(tag::distance));
		if (val < 0)
			throw FormulaV4aException("Inputs do not result in a valid solution for initial velocity");
		set(tag::initial_velocity, sqrt(val));
		set(tag::time, get_time_using_vel_acc_or_dist());
	}

	void compute_time_final_velocity() 
	{ 
		const double val = get(tag::final_velocity) + 2 * (get(tag::acceleration) * get(tag::distance));
		if (val < 0)
			throw FormulaV4aException("Inputs do not result in a valid solution for final velocity");
		set(tag::initial_velocity, sqrt(val));
		set(tag::time, get_time_using_vel_acc_or_dist());
	}

	void compute_time_acceleration() 
	{ 
		set(tag::acceleration, (pow(get(tag::final_velocity),2) - pow(get(tag::initial_velocity),2)) / (2 * get(tag::distance)));
		set(tag::time, get_time_using_vel_acc_or_dist());
	}

	void compute_initial_velocity_final_velocity()
	{
		if (get(tag::time) == 0)
			throw FormulaV4bException("Time cannot be zero when initial velocity and final velocity are unknown");
		set(tag::initial_velocity, (get(tag::distance) / get(tag::time)) - 0.5 * get(tag::acceleration) * get(tag::time));
		set(tag::final_velocity, get(tag::initial_velocity) + get(tag::acceleration) * get(tag::time));
	}

	void compute_initial_velocity_acceleration()
	{
		if (get(tag::time) == 0)
			throw FormulaV4bException("Time cannot be zero when initial velocity and acceleration are unknown");
		set(tag::initial_velocity, (2 * get(tag::distance)) / get(tag::time) - get(tag::final_velocity));
		if (get(tag::initial_velocity) == 0)
			throw FormulaV4aException("Inputs do not result in a valid solution for acceleration");
		set(tag::acceleration, (get(tag::final_velocity) - get(tag::initial_velocity)) / get(tag::time));
	}

	void compute_final_velocity_acceleration()
	{
		if (get(tag::time) == 0)
			throw FormulaV4bException("Time cannot be zero when final velocity and acceleration are unknown");
		set(tag::final_velocity, (2 * get(tag::distance)) / get(tag::time) - get(tag::initial_velocity));
		if (get(tag::final_velocity) == 0)
			throw FormulaV4aException("Inputs do not result in a valid solution for acceleration");
		set(tag::acceleration, (get(tag::final_velocity) - get(tag::initial_velocity)) / get(tag::time));
	}

	value_sequence    _values;		// equation variable values
	presence_sequence _presence;	// equation variable set/not set flags
	function_map      _functions;	// map of unknowns -> compute function
};
