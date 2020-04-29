#include "filehandler.h"

using namespace std;
string readInputFile(string filename) {
    ifstream File(filename);
    string code((istreambuf_iterator<char>(File)), istreambuf_iterator<char>());
    return code;
}

vector<string> splitC(string s, char delim)
{
    vector<string> v;
    stringstream stringstream1(s);
    string tmp;
    while (getline(stringstream1, tmp, delim)) {
        v.push_back(tmp);
    }
    return v;
}

string trim(string& str)
{
    string ret = str;
    int index = 0;
    //remove extra spaces and enters
    if (!ret.empty())
    {
        while ((index = ret.find(' ', index)) != string::npos)
        {
            ret = ret.erase(index, 1);
        }
    }
    index = 0;
    if (!ret.empty())
    {
        while ((index = ret.find('\n', index)) != string::npos)
        {
            ret = ret.erase(index, 1);
        }
    }
    index = 0;
    if (!ret.empty())
    {
        while ((index = ret.find('\t', index)) != string::npos)
        {
            ret = ret.erase(index, 1);
        }
    }
    index = 0;
    if (!ret.empty())
    {
        while ((index = ret.find('\f', index)) != string::npos)
        {
            ret = ret.erase(index, 1);
        }
    }
    index = 0;
    if (!ret.empty())
    {
        while ((index = ret.find('\v', index)) != string::npos)
        {
            ret = ret.erase(index, 1);
        }
    }
    return ret;
}

int isEmpty(string s)
{
    if (trim(s) == "")
        return 1;
    return 0;
}

unsigned int c_in_str(const char * str, char ch)
{
    unsigned int count = 0;

    while (*str)  //字符串本身自己带有空值字符
    {
        if (*str == ch)
            count++;
        str++;
    }
    return count;
}

//transform string to 8 digits hex number
string StrToHex(const string str)
{
    string tmp = str;
    if (tmp.length() > 32)
        tmp = str.substr(0, 32);
    char res[9];
    string ret = "";
    memset(res, '0', 9);
    for (int i = 7; i >= (7 - (ceil(tmp.length() / 4.0) - 1)); --i)
    {
        unsigned int hexword;
        if (i == 7)
            hexword = strtoul(tmp.substr(i * 4, tmp.length() - i * 4).c_str(), NULL, 2);
        else
            hexword = strtoul(tmp.substr(i * 4, 4).c_str(), NULL, 2);
        //cout << "tmphex: " << tmp.substr(i * 4, 4) << "*number:"<< hexword << endl;
        switch (hexword)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            res[i] = hexword + '0' - 0;
            break;
        case 10:
            res[i] = 'A';
            break;
        case 11:
            res[i] = 'B';
            break;
        case 12:
            res[i] = 'C';
            break;
        case 13:
            res[i] = 'D';
            break;
        case 14:
            res[i] = 'E';
            break;
        case 15:
            res[i] = 'F';
            break;
        };
    }
    for (int i = 0; i <= 7; i++)
    {
        ret += res[i];
    }
    return ret;
}

void getRidOfNotes(string& ret)
{
    int index = 0;
    //remove notations begin with "#"
    if (!ret.empty())
    {
        while ((index = ret.find("#", index)) != string::npos)
        {
            if (ret.find("\n") == string::npos)
            {
                ret = ret.erase(index, ret.length() - index);
            }
            else
            {
                ret = ret.erase(index, ret.find_first_of("\n", index) - index);
            }
        }
    }
}

//二进制字符串转化成十进制数
unsigned int ubinStr2dex(string ubs){
    unsigned int res = 0;
    res = strtoul(ubs.c_str(),NULL,2);
    return res;
}

int binStr2dex(string bs){
    int res = 0;
    //bs是补码表示的
    res =strtol(bs.c_str(),NULL,2);
    return res;
}

string dex2str(int num){
    char res[20];
    itoa(num,res,10);
    return res;
}

string udex2str(unsigned int num){
    char res[20];
    char tmpres[20];
    int i,j;
    i = 0;
    do{
        tmpres[i++] = num%10 + '0';
        num = num/10;
    }while(num);
    tmpres[i] = '\0';
    j=0;
    for(i = i-1;i>=0;i--){
        res[j++] = tmpres[i];
    }
    res[j] = '\0';
    return res;
}

static long long getValue(string s)
{
    s = trim(s);
    long long value;
    if(s.substr(0,2) == "0x" || s.substr(0,2) == "0X"){
        //s为十六进制字符串
         value = strtoll((s.substr(2)).c_str(),NULL,16);
    }
    else{
        //默认为十进制字符串
        value = strtoll(s.c_str(),NULL,10);
    }
    return value;
}

int isWithinLimit(string s){
    long long value = getValue(s);
    if(value > 32767 || value <-32768){
        return 0;
    }
    else
        return 1;
}

int getHighBits(string s){
    long long value = getValue(s);
    value = value >> 16;
    int v = value&0x0000FFFF;
    return v;
}

int getLowBits(string s){
    long long value = getValue(s);
    int v = value&0x0000FFFF;
    return v;
}

int containLetter(string s){
    s = trim(s);
    if(s.length() > 2 && s.substr(0,2) == "0x")
        return 0;
    for(int i = 0; i < strlen(s.c_str());i++){
        if((s[i] <= 'z' && s[i] >= 'a') || (s[i] >= 'A' && s[i] <= 'Z'))
            return 1;
    }
    return 0;
}

vector<string> splitElement(string s){
    vector<string> res;
    if(s.find(',') == string::npos){
        res.push_back(s);
        return res;
    }
    vector<string> tmpres = splitC(s,',');
    string ele = "";
    for(int i = 0; i<tmpres.size();i++){
        if(tmpres[i].length()){
            if(tmpres[i][0] == '\''&&tmpres[i][tmpres[i].length()-1] != '\''){
                ele += tmpres[i] + ",";
            }
            else if(tmpres[i][0] == '\''&&tmpres[i][tmpres[i].length()-1] == '\''){
                ele = tmpres[i];
                res.push_back(ele);
            }
            else if(tmpres[i][0] != '\''&&tmpres[i][tmpres[i].length()-1] == '\''){
                ele += tmpres[i];
                res.push_back(ele);
            }
            else if(tmpres[i].find('\'') == string::npos){
                ele = tmpres[i];
                res.push_back(ele);
            }
        }
    }
    return res;
}

int getVariableType(string v){
    if(v.find('\'')==string::npos)
        return 0;//NUM
    else if(v.length()==3 && v[0]=='\'' && v[2] =='\'')
        return 2; //CHAR
    else
        return 1;//STR
}

string udex2binstr(unsigned int num){
    char res[33];
    char tmpres[33];
    int i = 0;
    int j = 0;
    do{
        if(num%2==0)
            tmpres[i++] = '0';
        else
            tmpres[i++] = '1';
        num /= 2;
    }while(num);
    tmpres[i] = '\0';
    memset(res,'0',sizeof(char)*33);
    j = 32 - i;
    for(i = i - 1; i >= 0 && j < 32; i--){
        res[j++] = tmpres[i];
    }
    res[j] = '\0';
    string ret(res);
//    printf("Ret: %s\n",ret);
    return ret;
}
