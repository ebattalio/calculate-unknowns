// formula.h
#pragma once
#include <exception>
#include <string>
#include <array>
#include <sstream>
#include <unordered_map>
#include <functional>

const int VARIABLES = 5;

enum class ValueId: int
{
	distance,
	time,
	initial_velocity,
	final_velocity,
	acceleration
};

// specialize std::hash to support hashing a pair of ValueId
namespace std
{
	template <>
	class hash<std::pair<ValueId, ValueId>>
	{
	public:
		size_t operator()(const std::pair<ValueId, ValueId> &p) const
		{
			return hash<ValueId>()(p.first) ^ hash<ValueId>()(p.second);
		}
	};
};

//FormulaV4aException communicates parse and calculation errors back to the main program
class FormulaV4aException: public std::exception
{
public:
	FormulaV4aException(std::string message): msg_(message)
	{
	}

	std::string message() const
	{
		return msg_;
	}

private:
	std::string msg_;
};

class FormulaV4a
{
public:
	FormulaV4a(): formulas_(init_formulas())
	{
	}
	
	void reset()
	{
		for (auto &val : values_.variables_)
		{
			val.is_blank = true;
			val.v = 0.0;
		}
	}

	void set(ValueId id, double value)
	{
		values_.set(id, value);
	}

	double get(ValueId id) const
	{
		return values_.get(id);
	}

	void calculate()
	{
		std::vector<ValueId> unknowns;

		// find the unknowns
		for (int id = 0; id < VARIABLES; ++id)
		{
			if (values_.is_blank(static_cast<ValueId>(id)))
			{
				unknowns.push_back(static_cast<ValueId>(id));
			}
		}

		// two and only two fields should be blank
		if (unknowns.size() > 2)
		{
			throw FormulaV4aException("Error: more than two blank fields.");
		}
		if (unknowns.size() < 2)
		{
			throw FormulaV4aException("Error: less than two blank fields.");
		}

		// find the right formula
		auto formula_ptr = formulas_.find(std::make_pair(unknowns[0], unknowns[1]));
		if (formula_ptr == formulas_.end())
		{
			throw FormulaV4aException("Error: did not find a valid combination of two empty fields.");
		}
		auto formulaV4a = formula_ptr->second;

		// call the formula
		formulaV4a(values_);

		// now all fields should be set
		for (auto &val : values_.variables_)
		{
			val.is_blank = false;
		}
	}

private:
	struct Value
	{
		Value(): v(0.0), is_blank(true)
		{
		}

		double v;
		bool is_blank;
	};


	struct Values
	{
		void set(ValueId id, double value)
		{
			auto &val = variables_.at(static_cast<int>(id));
			val.v = value;
			val.is_blank = false;
		}

		double get(ValueId id) const
		{
			return variables_.at(static_cast<int>(id)).v;
		}

		double& at(ValueId id)
		{
			return variables_.at(static_cast<int>(id)).v;
		}

		bool is_blank(ValueId id) const
		{
			return variables_.at(static_cast<int>(id)).is_blank;
		}

		double& distance()
		{
			return at(ValueId::distance);
		}

		double& time()
		{
			return at(ValueId::time);
		}

		double& initial_velocity()
		{
			return at(ValueId::initial_velocity);
		}

		double& final_velocity()
		{
			return at(ValueId::final_velocity);
		}

		double& acceleration()
		{
			return at(ValueId::acceleration);
		}

		// more helpers
		double at2() const  //acceleration * time^2
		{
			return get(ValueId::acceleration) * pow(get(ValueId::time), 2);
		}

		double ad() const  //acceleration * distance
		{
			return get(ValueId::acceleration) * get(ValueId::distance);
		}

		double initial_velocity2() const  //vi^2
		{
			return pow(get(ValueId::initial_velocity), 2);
		}

		double final_velocity2() const  //vf^2
		{
			return pow(get(ValueId::final_velocity), 2);
		}

		void calculate_distance()
		{
			distance() = initial_velocity() * time() + 0.5 * at2();
		}

		void calculate_time()
		{
			if (acceleration() == 0.0)
			{
				if (final_velocity() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - final velocity cannot be zero.");
				}
				time() = distance() / final_velocity();
			}
			else
			{
				if (acceleration() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - acceleration cannot be zero.");
				}
				time() = (final_velocity() - initial_velocity()) / acceleration();
			}
		}

		std::array<Value, VARIABLES> variables_;
	};

	typedef std::unordered_map<std::pair<ValueId, ValueId>, std::function<void (Values&)>> Formulas;

	Values values_;
	Formulas formulas_;

	static Formulas init_formulas()
	{
		Formulas formulas;

		// if distance if the first incognita...
		formulas[std::make_pair(ValueId::distance, ValueId::time)] =
			[](Values &v)
			{
				if (v.acceleration() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - acceleration cannot be zero.");
				}
				v.time() = (v.final_velocity() - v.initial_velocity()) / v.acceleration();
				v.calculate_distance();
			};
		formulas[std::make_pair(ValueId::distance, ValueId::initial_velocity)] =
			[](Values &v)
			{
				v.initial_velocity() = v.final_velocity() - v.acceleration() * v.time();
				v.calculate_distance();
			};
		formulas[std::make_pair(ValueId::distance, ValueId::final_velocity)] =
			[](Values &v)
			{
				v.final_velocity() = v.initial_velocity() + v.acceleration() * v.time();
				v.calculate_distance();
			};
		formulas[std::make_pair(ValueId::distance, ValueId::acceleration)] =
			[](Values &v)
			{
				if (v.time() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - time cannot be zero.");
				}
				v.acceleration() = (v.final_velocity() - v.initial_velocity()) / v.time();
				v.calculate_distance();
			};

		// if time is the first incognita...
		formulas[std::make_pair(ValueId::time, ValueId::initial_velocity)] =
			[](Values &v)
			{
				double temp = v.final_velocity2() - 2 * v.ad(); // vi^2 = vf^2 - 2ad
				if (temp < 0)
				{
					throw FormulaV4aException("Error: inputs do not produce a valid solution.");
				}
				v.initial_velocity() = sqrt(temp);
				v.calculate_time();
			};
		formulas[std::make_pair(ValueId::time, ValueId::final_velocity)] =
			[](Values &v)
			{
				double temp = v.initial_velocity2() + 2 * v.ad();
				if (temp < 0)
				{
					throw FormulaV4aException("Error: inputs do not produce a valid solution.");
				}
				v.final_velocity() = sqrt(temp);
				v.calculate_time();
			};
		formulas[std::make_pair(ValueId::time, ValueId::acceleration)] =
			[](Values &v)
			{
				if (v.distance() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - distance cannot be zero.");
				}
				v.acceleration() = (v.final_velocity2() - v.initial_velocity2()) / (2 * v.distance());
				v.calculate_time();
			};

		// both initial_velocity is the first incognita...
		formulas[std::make_pair(ValueId::initial_velocity, ValueId::final_velocity)] =
			[](Values &v)
			{
				if (v.time() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - time cannot be zero.");
				}
				v.initial_velocity() = (v.distance() / v.time()) - 0.5 * v.acceleration() * v.time();
				v.final_velocity() = v.initial_velocity() + v.acceleration() * v.time();
			};
		formulas[std::make_pair(ValueId::initial_velocity, ValueId::acceleration)] =
			[](Values &v)
			{
				if (v.time() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - time cannot be zero.");
				}
				v.initial_velocity() = (2 * v.distance()) / v.time() - v.final_velocity();
				if (v.initial_velocity() < 0)
				{
					throw FormulaV4aException("Error: inputs do not produce a valid solution.");
				}
				v.acceleration() = (v.final_velocity() - v.initial_velocity()) / v.time();
			};

		// last case is final_velocity and acceleration...
		formulas[std::make_pair(ValueId::final_velocity, ValueId::acceleration)] =
			[](Values &v)
			{
				if (v.time() == 0.0)
				{
					throw FormulaV4aException("Error: divide by zero - time cannot be zero.");
				}
				v.final_velocity() = (2 * v.distance()) / v.time() - v.initial_velocity();
				if (v.final_velocity() < 0)
				{
					throw FormulaV4aException("Error: inputs do not produce a valid solution.");
				}
				v.acceleration() = (v.final_velocity() - v.initial_velocity()) / v.time();
			};

		return formulas;
	}
};
