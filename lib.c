static int testvar = 0x689a;
void selftest()
{
	int* t = &testvar;

	if(*t != 0x689a) {
		printcharadv('E', 0x4);
		while(1);
	} else {
		(*t)++;
		if(*t != 0x689b) {
			printcharadv('E', 0x2);
			while(1);
		}
	}
}


int min(x,y)
int x;
int y;
{
	return (x<y)?x:y;
}

void printstr(str, cur, attr)
char * str;
int cur;
int attr;
{
	setcur(cur);
	while(*str)
	{
		printcharadv(*str,attr);
		str++;
	}
}

int max(x,y)
int x;
int y;
{
	return (x>y)?x:y;
}

int myabs(x)
int x;
{
	return (x>0)?x:(-x);
}

static long s = 34534;
int myrand()
{
	s = s * 23523 + 12311;
	return (s % 7919) & 0xff;
}

int printlong(n, attr, minlen)
unsigned long n;
char attr;
int minlen;
{
	char st[140];
	int i = 0;

	do {
		st[i] = n%10;
		n = n/10;
		i++;
	} while (n != 0 || i<minlen);

	i--;
	while(i>=0) {
		printcharadv(st[i]+'0', attr);
		i--;
	}
}

