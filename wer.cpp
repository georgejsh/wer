/* File calculate WER and SER , needs orginal transcript and asr transcript */
//Cloned from https://github.com/alokprasad/asr-wer/blob/master/calculate-wer.cpp
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <fstream>
#include <streambuf>
using namespace std;
typedef vector<wstring> word_l;
typedef vector<word_l> Sentence;
bool operator==(const word_l & lhs, const word_l& rhs){
    if(lhs.size()==1 && rhs.size()==1) return lhs[0]==rhs[0];
    if(lhs.size()>1){
        for(auto w:lhs){
            if(w==rhs[0]) return true;
        }
        return false;
    }else{
        for(auto w:rhs){
            if(w==lhs[0]) return true;
        }
        return false;
    }
}
/*
Sentence &split(const wstring &s, wchar_t delim, Sentence &elems)
{
    wstringstream ss(s);
    wstring item;
    while (getline(ss, item, delim))
    {
        elems.push_back(item);
    }
    return elems;
}

Sentence split(const wstring &s, wchar_t delim)
{
    Sentence elems;
    split(s, delim, elems);
    return elems;
}

unsigned int countw(string const& str)
{
    stringstream stream(str);
    return distance(istream_iterator<string>(stream), istream_iterator<string>());
}
*/
unsigned int edit_distance(const Sentence &s1, const Sentence &s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    vector<vector<unsigned int>> d(len1 + 1, vector<unsigned int>(len2 + 1));

    d[0][0] = 0;
    for (unsigned int i = 1; i <= len1; ++i)
        d[i][0] = i;
    for (unsigned int i = 1; i <= len2; ++i)
        d[0][i] = i;

    for (unsigned int i = 1; i <= len1; ++i)
        for (unsigned int j = 1; j <= len2; ++j)
        {
            d[i][j] = min(d[i - 1][j] + 1, d[i][j - 1] + 1);
            d[i][j] = min(d[i][j], d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
        }
    return d[len1][len2];
}
wstring s2ws(const string &s)
{
    string curLocale = setlocale(LC_ALL, "");
    const char *_Source = s.c_str();
    size_t _Dsize = mbstowcs(NULL, _Source, 0) + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest, _Source, _Dsize);
    wstring result = _Dest;
    delete[] _Dest;
    setlocale(LC_ALL, curLocale.c_str());
    return result;
}
wstring trim(const wstring &s)
{
    int curr = 0, val = 0;
    bool space = false;
    while (s[curr] != '\0' && s[curr] == ' ')
        curr++;
    wstring res(s.length() * 2, ' ');
    while (s[curr] != '\0')
    {
        if (s[curr] != ' ' && !isalnum(s[curr]))
        {
            if (!space)
                res[val++] = ' ';
            res[val++] = s[curr++];
            res[val++] = ' ';
            space = true;
        }
        else if (s[curr] != ' ')
        {
            res[val++] = s[curr++];
            space = false;
        }
        else if (!space)
        {
            res[val++] = ' ';
            space = true;
        }
        else
            curr++;
    }
    if (space)
        val--;
    res.resize(val);
    return res;
}
word_l find_word(const wstring &s, size_t start, size_t end){
    word_l list_w;
    wstringstream ss(s.substr(start,end)+L'^');
   // wcout<<"String:"<<end<<"xxx "<<s.substr(start,end)<<endl;
    wstring item;
    while (getline(ss, item, L'^'))
    {
        item=trim(item);
        list_w.push_back(item);
    }
    return list_w;
}
Sentence parse_to_sentence(const wstring &s)
{
    size_t curr = 0, val = 0;
    bool space = false;
    while (s[curr] != '\0' && s[curr] == ' ') curr++;
    Sentence result;
    word_l w_l;
    wstring word;
    while (s[curr] != '\0')
    {
        if(s[curr]=='['){
            if(word.size()>0) {
                w_l.push_back(word);
                result.push_back(w_l);
                word=L"";
                w_l.clear();
            }
            size_t  found = s.find(']', curr+1);
            if (found == wstring::npos) return result;
            result.push_back(find_word(s,curr+1,found-curr-1));
            space=true;
            curr=found+1;
        }
        else if (s[curr] != ' ' && !isalnum(s[curr]))
        {
            if(word.size()>0) {
                w_l.push_back(word);
                result.push_back(w_l);
                word=L"";
                w_l.clear();
            }
            word=s[curr++];
            w_l.push_back(word);
            result.push_back(w_l);
            word=L"";
            w_l.clear();
            space=true;
        }
        else if (s[curr] != ' ')
        {
            word+=s[curr++];
            space = false;
        }
        else if (!space)
        {
            w_l.push_back(word);
            result.push_back(w_l);
            word=L"";
            w_l.clear();
            space=true;
            curr++;
        }
        else
            curr++;
    }
    if(word.size()>0){
        w_l.push_back(word);
        result.push_back(w_l);
    }
   return result;
}
void print_sentence(Sentence &s){
    //wcout<<"Sentence:";
    for(auto wl:s){
            if(wl.size()==1){
                wcout<<wl[0]<<" ";
            }else{
                wcout<<"[";
                for(int i=0;i<wl.size();i++)
                    if(i!=wl.size()-1)
                        wcout<<wl[i]<<",";
                    else
                        wcout<<wl[i];
                wcout<<"] ";
            }
        }
    wcout<<endl;
}
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        cout << "Usage:WER <org ref file> <asr result file>" << endl;
        return -1;
    }

    float wer = 0;
    float total_wc = 0;
    float total_line_no_match = 0;
    float total_line = 0;

    ifstream org_file(argv[1]);
    ifstream asr_file(argv[2]);

    string org;
    string asr;

    Sentence s1;
    Sentence s2;

    while (getline(org_file, org))
    {
        getline(asr_file, asr);
        //wstring org_line = trimAll(s2ws(org));
        //wstring asr_line = trimAll(s2ws(asr));
        //wcout << org_line << endl;
        //wcout << asr_line << endl;
        Sentence s1=parse_to_sentence(s2ws(org));
        //print_sentence(s1);
        Sentence s2=parse_to_sentence(s2ws(asr));
       // print_sentence(s2);
        //continue;
       
        total_line++;

        
        int w = edit_distance(s1, s2);
        wer = wer + w;
        total_line_no_match+=(w>0?1:0);
        total_wc = total_wc + s1.size(); //countw(org_line);
        wcout << "Ref: " ;
        print_sentence(s1);
        wcout << "Hyp: " ;
        print_sentence(s2);
        wcout << "WER: " << w << " SER: " << (w>0?1:0) << endl;
    }

    wcout << "FINAL WER = " << wer / total_wc << "   ";
    wcout << "FINAL SER = " << total_line_no_match / total_line << endl;
    return 0;
}