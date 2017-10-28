#include "interpolator.h"

#include <math.h>
#include <iostream>
#include <vector>
#include <algorithm>

struct sorter point_sorter;

point::point(double x, double y)
{
	this->x = x;
	this->y = y;
}

interpolator::interpolator() :isCached(false), use_out_value(false)
{}

interpolator::interpolator(std::vector < double> xx, std::vector<double> yy, int ord, int ordered) : interpolator()
{
	initialize(xx, yy, ord, ordered);
}

interpolator::~interpolator()
{}

void interpolator::initialize(std::vector < double> xx, std::vector<double> yy, int ord, int ordered)
{
	isCached = false;
	ps.clear();
	coefs.clear();
	if (-1 == ordered)
	{
		auto i = xx.rbegin();
		auto j = yy.rbegin();
		for (; (i != xx.rend() && (j != yy.rend())); ++i, ++j)
			ps.push_back(point(*i, *j));
		order = (ord + 1) < ps.size() ? ord : ps.size() - 1;
		return;
	}
	auto i = xx.begin();
	auto j = yy.begin();
	for (; (i != xx.end() && (j != yy.end())); ++i, ++j)
		ps.push_back(point(*i, *j));
	if (1 != ordered)
		std::sort(ps.begin(), ps.end(), point_sorter);
	order = (ord + 1) < ps.size() ? ord : ps.size() - 1;
}

void interpolator::set_order(int ord)
{
	isCached = false;
	order = ord;
}
void interpolator::enable_out_value(double val)
{
	use_out_value = true;
	out_value = val;
}
void interpolator::disable_out_value()
{
	use_out_value = false;
}

double interpolator::operator()(double point)
{
	if (use_out_value)
		if ((point < ps.front().x) || (point>ps.back().x))
			return out_value;
	int n_min, n_max;
	get_indices(point, n_min, n_max);
	if (isCached)
		if ((n_min == cache_nx_from) && (n_max == cache_nx_to))
			return calculate(point); //same polynom is used
	cache_nx_from = n_min;
	cache_nx_to = n_max;
	isCached = true;
	build_coefs();
	return calculate(point);
}

void interpolator::get_indices(double x, int &n_min, int &n_max)
{
	if (x < ps.front().x)
	{
		n_min = 0; 
		n_max = order; //order is set to be matching the number of points
		return;
	}
	if (x > ps.back().x)
	{
		n_max = ps.size() - 1;
		n_min = n_max - order;
		return;
	}
	int out_ = 0;
	while ((x > ps[out_].x) && (x > ps[out_+1].x)) out_++;
	n_min = out_ - order / 2; //assymetrical interpolation in the case of odd order.
	n_max = n_min + order;
	if (n_min < 0)
	{
		n_min = 0;
		n_max = n_min + order;
	}
	if (n_max >= ps.size())
	{
		n_max = ps.size() - 1;
		n_min = n_max - order;
	}
}

void interpolator::build_coefs(void)
{
	coefs.clear();
	for (int ss = 0; ss <= order; ss++)	coefs.push_back(ps[ss+cache_nx_from].y);
	std::vector<std::vector<double>> matrix;
	matrix.push_back(coefs);
	for (int OOk = 1; OOk <=order; OOk++)
		matrix.push_back(std::vector<double>());
	for (int OOk = 1; OOk <= order; OOk++)
		for (int kok = 0; kok <= order - OOk; kok++)
			matrix[OOk].push_back((matrix[OOk - 1][kok] - matrix[OOk-1][kok+1]) / (ps[kok+cache_nx_from].x - ps[kok + OOk+cache_nx_from].x));
	for (int OOk = 1; OOk <= order; OOk++)
		matrix[0][OOk] = matrix[OOk][0];
	coefs = matrix[0];
}

double interpolator::calculate(double x)
{
	double out_ = coefs[order];
	for (int o_O = 0; o_O < order; o_O++) 
		out_ = coefs[order - 1 - o_O] + (x - ps[order - 1 - o_O+cache_nx_from].x)*out_;
	return out_;
}
