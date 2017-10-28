#include <iostream>
#include <fstream>
#include <string>
#include "interpolator.h"

static std::string str_token_data;
std::string strtoken(std::string in, std::string break_symbs)
{
	if (!in.empty())
		str_token_data = in;
	std::string out;
	while (!str_token_data.empty())
	{
		char a = str_token_data.front();
		str_token_data.erase(str_token_data.begin());
		bool break_ = false;
		for (auto h = break_symbs.begin(); h != break_symbs.end(); h++)
			if (a == *h)
			{
				break_ = true;
				break;
			}
		if ((break_) && (out.empty()))
			continue;
		if (break_)
			return out;
		out.push_back(a);
	}
	return out;
}

int main(int argc, char ** argv)
{
	std::cout << "---------TESTING STRTOKEN----------" << std::endl;
	std::string test_str = "This is , a test	string... Yeah.";
	test_str = strtoken(test_str, "\t .");
	std::cout << test_str << std::endl;
	while (!test_str.empty())
	{
		test_str = strtoken("", "\t .,");
		std::cout << test_str << std::endl;
	}
	std::cout << "---------FINISHED TESTING STRTOKEN----------" << std::endl;

	std::ifstream drift_data;
	drift_data.open("drift_data.txt",std::ios_base::in);
	std::string line;
	std::vector <double> x, y;
	std::getline(drift_data,line);
	while (!drift_data.eof())
	{
		std::getline(drift_data, line);
		line=strtoken(line,"\t ");
		if (line.empty())
			break;
		x.push_back(std::stod(line));
		strtoken("", "\t ");
		line = strtoken("", "\t ");
		if (line.empty())
			break;
		y.push_back(std::stod(line));
	}
	drift_data.close();

	drift_data.open("diffusion_data.txt",std::ios_base::in);
	std::vector <double> x_out, y_out;
	std::getline(drift_data, line);
	while (!drift_data.eof())
	{
		std::getline(drift_data, line);
		line = strtoken(line, "\t ");
		if (line.empty())
			break;
		x_out.push_back(std::stod(line));
		line = strtoken("", "\t ");
		if (line.empty())
			break;
		y_out.push_back(std::stod(line));
	}
	drift_data.close();

	//done:TODO: add test of strtoken and interpolation here:
	//orders 1, 4 and 5 with different sizes (check too few point case)
	//+ outside test? + matching the 1st and last points with required one

	interpolator inst;
	inst.initialize(x, y, 4, 1);

	std::cout << "------TESTING INTERPOLATION-------" << std::endl;
	inst(x.front());
	inst(x.back());
	std::ofstream test("A1.txt", std::ios_base::trunc);
	for (int h = 0; h < x.size(); h++)
		test << x[h] << "\t" << y[h] << std::endl;
	test.close();
	test.open("A2.txt", std::ios_base::trunc);
	double temp;
	for (int h = 0; h < 3 * x.size(); h++)
	{
		temp = x.front() + (h)*(x.back() - x.front()) / (3 * x.size() - 1);
		test << temp << "\t" << inst(temp) << std::endl;
	}
	test.close();
	test.open("A3.txt", std::ios_base::trunc);
	inst.set_order(5);
	for (int h = 0; h < 3 * x.size(); h++)
	{
		temp = x.front() + (h)*(x.back() - x.front()) / (3 * x.size() -1);
		test << temp << "\t" << inst(temp) << std::endl;
	}
	test.close();
	std::vector<double> xt, yt;
	xt.push_back(0);
	yt.push_back(1);
	xt.push_back(1);
	yt.push_back(2);
	inst.initialize(xt, yt, 2, 1);
	std::cout << "order 2 at 2 points" << std::endl;
	std::cout<<"(-1, "<< inst(-1)<<")\t(0, "<<inst(0)<<")\t(0.5, "<<inst(0.5)<<")\t(1, "<<inst(1)<<")\t(1.5, "<<inst(1.5)<<")\n";
	xt.pop_back();
	yt.pop_back();
	inst.initialize(xt, yt, 2, 1);
	std::cout << "order 2 at 1 point" << std::endl;
	std::cout << "(-1, " << inst(-1) << ")\t(0, " << inst(0) << ")\t(0.5, " << inst(0.5) << ")\n";
	std::cout << "------FINISHED TESTING INTERPOLATION-------" << std::endl;

	inst.initialize(x,y,6,1);
	std::ofstream diffus_data;
	diffus_data.open("diff_data.txt",std::ios_base::trunc);
	diffus_data << "E[kV/cm]\tD/mu [V]\tD[cm^2/sec]\n";
	for (int h = 0; h < x_out.size(); h++)
	{
		diffus_data << x_out[h]<<"\t";
		diffus_data << y_out[h] << "\t";
		diffus_data << y_out[h]*inst(x_out[h])*0.001 << "\n";
	}
	diffus_data.close();
	std::getchar();
	return 0;
}
