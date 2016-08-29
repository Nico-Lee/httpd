#include<iostream>
#include<string>
#include<assert.h>
#include<limits.h>
#define INT64 long long int
using namespace std;

class BigData
{
public:
	BigData(INT64 data);
	BigData(const char *str);
	BigData operator+(BigData& d);
	BigData operator-(BigData& d);
	BigData operator*(BigData& d);
	BigData operator/(BigData& d);
private:
	friend string Add(string& left, string& right);
	friend string Sub(string& left, string& right);
	friend string Mul(string& left, string& right);
	friend string Div(string& left, string& right);
	bool _IsINT64OverFlow()const;
	friend ostream& operator<<(ostream& _cout, const BigData& d);
	void _INT64ToString();
private:
	string _strvalue;
	INT64 _value;
};

bool OverflowFlag = false;



BigData::BigData(INT64 data)
	:_value(data)
{
	_INT64ToString();
}
BigData::BigData(const char *str)
	: _value(0)
{
	if (str == NULL)
	{
		assert(false);
		return;
	}
	char symbol;
	if (str[0] == '+')
	{
		symbol = '+';
		str++;
	}
	else if (str[0] == '-')
	{
		symbol = '-';
		str++;
	}
	else if (str[0] >= '0'&&str[0] <= '9')
	{
		symbol = '+';
	}
	else
	{
		return;
	}
	char* tmpstr = (char*)str;
	while (*tmpstr == '0')//跳过前面的‘0’
		tmpstr++;
	int i = 0;//剩下字符串的长度
	while (*tmpstr >= '0'&& *tmpstr <= '9')
	{
		i++;
		_value = _value * 10 + *tmpstr - '0';
		tmpstr++;
	}
	if (symbol == '-')
	{
		_value = 0 - _value;
	}


	_strvalue.resize(i + 1);//相当于给_strvalue开辟空间
	_strvalue[0] = symbol;
	int j = 1;
	while (i--)
	{
		_strvalue[j++] = *str++;//*tmpstr++
	}

}


bool BigData:: _IsINT64OverFlow()const
{
	if (_value >= LLONG_MIN && _value <= LLONG_MAX)
		return false;
	return true;
}
ostream& operator<<(ostream& _cout,const BigData& d)
{
	if (OverflowFlag)
	{
		if (d._strvalue[0] == '-')
			_cout << '-';
		size_t i = 1;
		while (d._strvalue[i] == '0')
			i++;
		for (; i < d._strvalue.size(); ++i)
		{
			_cout << d._strvalue[i];
		}
	}
	else
	{
		_cout << d._value;
	}
	return _cout;
}
void BigData::_INT64ToString()
{
	INT64 tmp = _value;
	INT64 sym = tmp;
	string str;
	if (sym >= 0)
	{
		str.push_back('+');
	}
	else
	{
		str.push_back('-');
		tmp = 0 - tmp;
	}
	while (tmp)
	{
		char ctmp = tmp % 10 + '0';
		str.push_back(ctmp);
		tmp /= 10;
	}
	//现在的数据是反的，需要从头到尾交换顺序
	int right = str.size()-1;
	int left = 1;
	while (left < right)
	{
		swap(str[left++], str[right--]);
	}
	_strvalue = str;
}

//比较两个字符串的大小，除法
bool Compare(string& left, string& right)
{
	size_t leftsize = left.size();
	size_t rightsize = right.size();
	int i = 1;
	int j = 1;
	while (left[i] == '0')
	{
		i++;
		leftsize--;
	}
	while (right[j] == '0')
	{
		j++;
		rightsize--;
	}
	if (leftsize > rightsize)
		return true;
	else if (rightsize > leftsize)
		return false;
	else
	{
		for (; i < left.size(), j < right.size(); i++, j++)
		{
			if (left[i] > right[j])
				return true;
			if (right[j]>left[i])
				return false;
		}
		return true;
	}
}


string Sub(string& left, string& right);
string Div(string& left, string& right)
{
	string newstr;//创建一个临时sting存放相除后的结果
	int lsize = left.size();
	int rsize = right.size();
	newstr.resize(lsize);
	if (left[0] != right[0])
	{
		newstr[0] = '-';
	}
	else
	{
		newstr[0] = '+';
	}
	if (lsize < rsize)
	{
		newstr.push_back('0');
		return newstr;
	}
	else
	{
		left[0] = '+';
		right[0] = '+';
		int i = 0;
		int flag = rsize;
		int j = 0;
		string tmp;
		tmp.resize(rsize);
		while (j < flag)//将left的高位复制给临时变量
		{
			tmp[j] = left[j];
			j++;
		}
		j--;
		while (j < lsize)
		{
			newstr[j] = '0';
			while (Compare(tmp, right))
			{
				newstr[j]++;
				tmp = Sub(tmp, right);
			}
			tmp.push_back(left[++j]);
		}
		return newstr;

	}
}

string Sub(string& left, string& right);
string Add(string& left, string& right)
{
	if (left[0] != right[0])//符号不等
	{
		if (left[0] == '+')
		{
			right[0] = '+';
			return Sub(left, right);
		}
		else
		{
			left[0] = '+';
			return Sub(right, left);
		}
	}
	else
	{
		int lsize = left.size();
		int rsize = right.size();
		if (lsize == rsize)
		{
			int carry = 0;
			while (--lsize && --rsize)
			{
				char tmp = left[lsize];
				left[lsize] = (left[lsize] - '0' + right[rsize] - '0') % 10 + carry + '0';
				carry = (tmp - '0' + right[rsize] - '0') / 10;
			}
			if (carry == 1)
			{
				left.insert(1, "1");
			}
			return left;
		}
		else
		{
			if (lsize > rsize)
			{
				int carry = 0;//进位
				while (--lsize && --rsize)//不能为--rsize&&-lsize
				{
					char tmp = left[lsize];
					left[lsize] = (left[lsize] - '0' + right[rsize] - '0') % 10 + carry + '0';
					carry = (tmp - '0' + right[rsize] - '0') / 10;
				}
				while (carry == 1)
				{
					left[lsize] = left[lsize] + carry;
					carry = (left[lsize] - '0' + carry) / 10;
					lsize--;
				}
				
				return left;
			}
			else
			{
				int carry = 0;
				while (--rsize && --lsize)//注意不能为--lsize&&--rsize，
					//当lsize为1时不执行--lsize直接跳出
				{
					char tmp = right[rsize];
					right[rsize] = (left[lsize] - '0' + right[rsize] - '0') % 10 
						+ '0' + carry;
					carry = (tmp - '0' + left[lsize] - '0') / 10;
				}
				while (carry == 1)
				{
					right[rsize] = right[rsize] + carry;
					carry = (right[rsize] - '0' + carry) / 10;
					rsize--;
				}
				return right;
			}
			
		}
	}
}
string Sub(string& left, string& right)
{
	if (left[0] != right[0])
	{
		if (left[0] == '+')
		{
			right[0] = '+';
			return Add(left, right);
		}
		else
		{
			right[0] = '-';
			return Add(left, right);
		}
	}
	else
	{
		int lsize = left.size();
		int rsize = right.size();
		if (lsize == rsize)
		{
			int borrow =0;
			while (--lsize && --rsize)
			{
				
				if (left[lsize] < right[rsize])
				{
					left[lsize] = left[lsize] + 10 - right[rsize] - borrow + '0';
					borrow = 1;
				}
				else
				{
					left[lsize] = left[lsize] - right[rsize] - borrow + '0';
					borrow = 0;
				}
			}
			return left;
		}
		else if (lsize > rsize)
		{
			int borrow = 0;
			while (--lsize && --rsize)
			{
				if (left[lsize] < right[rsize])
				{
					left[lsize] = left[lsize] + 10 - right[rsize] - borrow + '0';
					borrow = 1;
				}
				else
				{
					left[lsize] = left[lsize] - right[rsize] - borrow + '0';
					borrow = 0;
				}
			}
			while ( borrow==1 )
			{
				if (left[lsize] == '0')
				{
					left[lsize] = left[lsize] - '0' + 10 - borrow + '0';//若借位为0，
					//向更高位借位,eg:1000-10
					lsize--;
				}
				else
				{
					left[lsize] = left[lsize] - '0' - borrow + '0';
					borrow = 0;
				}

			}
			
			return left;
		}
		else
		{
			int borrow = 0;
			while (--rsize && --lsize)
				//得先让rsize--，若--lsize为0；将不会执行--rsize;
			{
				if (right[rsize] < left[lsize])
				{
					right[rsize] = right[rsize] + 10 - left[lsize] - borrow + '0';
					borrow = 1;
				}
				else
				{
					right[rsize] = right[rsize] - left[lsize] - borrow + '0';
					borrow = 0;
				}
			}
			while (borrow == 1)
			{
				if (right[rsize] == '0')
				{
					right[rsize] = right[rsize] - '0' + 10 - borrow + '0';//若借位为0，
					//向更高位借位,eg:1000-10
					rsize--;
				}
				else
				{
					right[rsize] = right[rsize] - '0' - borrow + '0';
					borrow = 0;
				}

			}
			return right;
		}
	}
}
string Mul(string& left, string& right)
{
	string newstr;//创建一个临时sting存放相乘后的结果
	int lsize = left.size();
	int rsize = right.size();
	newstr.resize(lsize + rsize);
	int newsize = newstr.size();
	while (--newsize)//初始化string，如果不初始化，string里存的是‘\0’
	{
		newstr[newsize] = '0';
	}
	if (left[0] != right[0])//符号不等
	{
		newstr[0] = '-';
	}
	else
	{
		newstr[0] = '+';
	}
	int flag = 0;//标志每次积的最低位
	int carry = 0;
	if (lsize <= rsize)
	{
		while (--lsize)
		{
			newsize = newstr.size() - flag;
			rsize = right.size();
			while (--rsize)
			{
				char tmp = left[lsize];
				newstr[--newsize] = ((left[lsize] - '0') * (right[rsize] - '0')
					+ newstr[newsize]-'0') % 10 + carry + '0';
				carry = ((tmp - '0') * (right[rsize] - '0')) / 10;
			}
			newstr[--newsize] = carry + '0';//把最后的进位存起来
			flag++;
		}
	}
	else
	{
		while (--rsize)
		{
			newsize = newstr.size() - flag;
			lsize = left.size();
			while (--lsize)
			{
				char tmp = left[lsize];
				newstr[--newsize] = ((left[lsize] - '0') * (right[rsize] - '0')
					+ newstr[newsize] - '0') % 10 + carry + '0';
				carry = ((tmp - '0') * (right[rsize] - '0')) / 10;
			}
			newstr[--newsize] = carry + '0';
			flag++;
		}
	}
	return newstr;
}

BigData BigData::operator+( BigData& d)
{

	if (!_IsINT64OverFlow() && !d._IsINT64OverFlow() 
		&& (_value + d._value) <= LLONG_MAX && (_value + d._value) >= LLONG_MIN)
	{
		_value += d._value;
	}
	else
	{
		OverflowFlag = true;
		_strvalue = Add(_strvalue, d._strvalue);
	}
	return *this;
}
BigData BigData::operator-( BigData& d)
{
	if (!_IsINT64OverFlow() && !d._IsINT64OverFlow()
		&& _value - d._value <= LLONG_MAX && _value - d._value >= LLONG_MIN)
	{
		_value -= d._value;
	}
	else
	{
		OverflowFlag = 1;
		_strvalue = Sub(_strvalue, d._strvalue);
	}
	return *this;
}
BigData BigData::operator*(BigData& d)
{
	if (!_IsINT64OverFlow() && !d._IsINT64OverFlow()
		&& _value * d._value <= LLONG_MAX && _value * d._value >= LLONG_MIN)
	{
		_value *= d._value;
	}
	else
	{
		OverflowFlag = 1;
		_strvalue = Mul(_strvalue, d._strvalue);
	}
	return *this;
}
BigData BigData::operator/(BigData& d)
{
	if (!_IsINT64OverFlow() && !d._IsINT64OverFlow()
		&& _value / d._value <= LLONG_MAX && _value / d._value >= LLONG_MIN)
	{
		_value /= d._value;
	}
	else
	{
		OverflowFlag = 1;
		_strvalue = Div(_strvalue, d._strvalue);
	}
	return *this;
}

