#include <opencv2/shape/emdL1.hpp>
#include <iostream>

int main()
{
	int R, C;
	std::cin >> R >> C;

	cv::Mat M(R, C, CV_32FC1);
	for (int r = 0; r < R; ++r)
	{
		for (int c = 0; c < C; ++c)
		{
			std::cin >> M.at<float>(r, c);
		}
	}

	cv::Mat M0(R, C, CV_32FC1, 0.0);
	std::cout << cv::EMDL1(M, M0) << std::endl;
	// not good, same as: std::cout << cv::EMDL1(M.col(0), M0.col(0)) << std::endl;
}
