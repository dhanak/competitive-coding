#import <bits/stdc++.h>
main(int n)
{
	std::cin >> n;
	std::cout << n-(2<<int(log2(n)))%n;
}
