#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H 
#include <math.h>
#include <vector>
#include <algorithm>

class point {
public:
	double x, y;
	point(double x, double y);
};

struct sorter {
	bool operator() (point a, point b) { return a.x < b.x; }
};

class interpolator {
public:
	interpolator();
	interpolator(std::vector < double> xx, std::vector<double> yy, int ord, int ordered = 0);
	~interpolator();

	void initialize(std::vector < double> xx, std::vector<double> yy, int ord, int ordered = 0);
	void set_order(int ord);
	void enable_out_value(double val);
	void disable_out_value();

	double operator()(double point);
protected:
	bool isCached;
	int cache_nx_from, cache_nx_to;

	double out_value;
	bool use_out_value;
	std::vector<point> ps;
	int order;

	std::vector<double> coefs;
	//double x_min, x_max, y_min, y_max;
	void build_coefs(void);
	void get_indices(double point, int &n_min, int &n_max); //[n_min, n_max] are used, not [n_min,n_max). order==n_max-n_min
	double calculate(double x);
};

#endif
