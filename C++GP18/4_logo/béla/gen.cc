#include <cassert>
#include <cstdio>
typedef unsigned int uint;

char const *logo=
	"`+syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyo:\n"
	"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy/\n"
	"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy+\n"
	"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy+\n"
	"syyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy+                                       .-:::::-.`\n"
	"syyyyyyyy+      `/yyyyyyyyyyyyy`      :yyyyyys:             yyyyyyy/      ./oyyyyyyyyyyys+:`\n"
	"syyyyyyyy+        `+yyyyyyyyyyy`      :yyyyyyyys-           yyyyyyy/   `/syyyyyyyyyyyyyyyyyyo-\n"
	"syyyyyyyy+          -syyyyyyyyy`      :yyyyyyyyyy+`         yyyyyyy/  :syyyyyyyyyyyyyyyyyyyyyyo`\n"
	"syyyyyyyy+            :syyyyyyy`      :yyyyyyyyyyyy/`       yyyyyyy/ +yyyyyyys/-`   `.:oyyyyyyys.\n"
	"syyyyyyyy+             `+yyyyyy`      :yyyyyyyyyyyyys-      yyyyyyy//yyyyyyy:           `////////\n"
	"syyyyyyyy+               .oyyyy`      :yyyyyyyyyyyyyyyo.    yyyyyyyoyyyyyyy.     `````````````````\n"
	"syyyyyyyy+       :         -syy`      :yyyyyyy/yyyyyyyyy+`  yyyyyyyyyyyyyy/      syyyyyyyyyyyyyyyy\n"
	"syyyyyyyy+       ys-         /y`      :yyyyyys .oyyyyyyyyy: yyyyyyyyyyyyyy/      syyyyyyyyyyyyyyyy\n"
	"syyyyyyyy+       yyyo.        `       :yyyyyys   -syyyyyyyysyyyyyyyoyyyyyys`     syyyyyyyyyyyyyyyo\n"
	"syyyyyyyy+       yyyyy+`              :yyyyyys     :syyyyyyyyyyyyyy/oyyyyyys.    ::::::::syyyyyyy-\n"
	"syyyyyyyy+       yyyyyyy/             :yyyyyys      `/yyyyyyyyyyyyy/`oyyyyyyy+-`      .:syyyyyyy:\n"
	"syyyyyyyy+       yyyyyyyys-           :yyyyyys        `+yyyyyyyyyyy/ `+yyyyyyyyysooosyyyyyyyyys-\n"
	"syyyyyyyy+       yyyyyyyyyyo.         :yyyyyys          .oyyyyyyyyy/   .oyyyyyyyyyyyyyyyyyyyy/`\n"
	"syyyyyyyy+       yyyyyyyyyyyy+`       :yyyyyys            :syyyyyyy/     ./oyyyyyyyyyyyyyy+:\n"
	"syyyyyyyo.       .............`       .:::::::             `:::::::.        `-:/++oo++/-.\n"
	"syyyyyo.\n"
	"syyy+.\n"
	"+s+.\n";

char const *A="y`. +-/os:\n";

int main()
{
	uint counts[128]={};
	printf(">");
	for(uint i=0; i<11; ++i)
	{
		char c=A[(i+9)%11];
		if(c=='\n')
			printf("\\n");
		else printf("%c",c);
	}
	for(char const *p=logo; *p;)
	{
		char c=*p;
		uint k=0;
		while(A[k] && A[k]!=c)
			++k;
		assert(A[k]);
		uint n=0;
		while(p[n]==c && n<9)
			++n;
		p+=n;
		counts[c]+=n;
		assert(1<=n && n<=9);
		int x=35+k+11*(n-1);
		assert(32<=x && x<128);
		//printf("%u %c %c %d\n",n,c,x,x);
		if(x=='"')
			printf("\\%c",x);
		else printf("%c",x);
	}
	//for(uint i=0; i<128; ++i)
	//	if(counts[i]!=0)
	//		printf("%u >%c< %u\n",i,i,counts[i]);
	printf("<\n");
	return 0;
}

/*
10 >
< 23
32 > < 958
43 >+< 37
45 >-< 16
46 >.< 36
47 >/< 35
58 >:< 56
96 >`< 52
111 >o< 25
115 >s< 57
121 >y< 888

*/