#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <ostream>
#include <sstream>
#include <cstdlib>

using namespace std;
//状态机中的终止状态编号
const vector<int> Fstate = {2,6,7,9,10,12,13,15,16,18,19,20,22,23,24,
                      26,27,29,30,31,32,33,34,35,36,37,38,40,42};

class LexAnalyzer{
private:
    int state; //当前状态
    bool error; //记录是否发生错误
    int ptr_i; //记录当前读到的字符下标
    string input; //保存读入的源程序字符串
    string str; //记录当前正在分析的单词


    vector<string> TOKEN_k; //关键字表
    vector<string> TOKEN_p; //界符表
    vector<string> TOKEN_i; //标识符表
    vector<string> TOKEN_c; //数字常量表
    vector<string> TOKEN_charc; //字符常量表
    vector<string> TOKEN_strc; //字符串常量表
    vector<pair<string,int>> ans; //结果表，以<类型，在表中的位置>的形式储存

public:
    LexAnalyzer(){
        state = 0;
        ptr_i = 0;
        error = false;
        //初始化关键字表
        TOKEN_k.emplace_back("int");
        TOKEN_k.emplace_back("main");
        TOKEN_k.emplace_back("void");
        TOKEN_k.emplace_back("if");
        TOKEN_k.emplace_back("else");
        TOKEN_k.emplace_back("char");
        TOKEN_k.emplace_back("float");
        TOKEN_k.emplace_back("double");
        TOKEN_k.emplace_back("for");
        TOKEN_k.emplace_back("while");
        TOKEN_k.emplace_back("break");
        TOKEN_k.emplace_back("switch");
        TOKEN_k.emplace_back("case");
        TOKEN_k.emplace_back("default");
        TOKEN_k.emplace_back("return");

        //初始化界符表
        TOKEN_p.emplace_back("<="); //0
        TOKEN_p.emplace_back(">="); //1
        TOKEN_p.emplace_back("=="); //2
        TOKEN_p.emplace_back("="); //3
        TOKEN_p.emplace_back(">"); //4
        TOKEN_p.emplace_back("<"); //5
        TOKEN_p.emplace_back("+"); //6
        TOKEN_p.emplace_back("++"); //7
        TOKEN_p.emplace_back("+="); //8
        TOKEN_p.emplace_back("-"); //9
        TOKEN_p.emplace_back("--"); //10
        TOKEN_p.emplace_back("-="); //11
        TOKEN_p.emplace_back("*"); //12
        TOKEN_p.emplace_back("*="); //13
        TOKEN_p.emplace_back("/"); //14
        TOKEN_p.emplace_back("/="); //15
        TOKEN_p.emplace_back("{"); //16
        TOKEN_p.emplace_back("}"); //17
        TOKEN_p.emplace_back("("); //18
        TOKEN_p.emplace_back(")"); //19
        TOKEN_p.emplace_back("["); //20
        TOKEN_p.emplace_back("]"); //21
        TOKEN_p.emplace_back(","); //22
        TOKEN_p.emplace_back(";"); //23
    }

    void read_str(const char* addr){ //读取输入的C程序文件
        ifstream in(addr);
        ostringstream tmp;
        tmp << in.rdbuf();
        input = tmp.str();
    }

    bool is_char(char c){ //判断该词是否为字符
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
            return true;
        else
            return false;
    }

    bool is_digit(char c){ //判断是否为数字
        if(c >= '0' && c <= '9')
            return true;
        else
            return false;
    }

    void next_state(char ch){ //自动机状态转移函数
        if(state == 0){
            if(is_char(ch)){state=1;str.push_back(ch);} // δ(0,字母)=1
            else if(is_digit(ch)){state=3;str.push_back(ch);} // δ(0,数字)=3
            else if(ch == '='){state=8;str.push_back(ch);} // δ(0,'=')=8
            else if(ch == '>'){state=11;str.push_back(ch);} // δ(0,'>')=11
            else if(ch == '<'){state=14;str.push_back(ch);} // δ(0,'<')=14
            else if(ch == '+'){state=17;str.push_back(ch);} // δ(0,'+')=17
            else if(ch == '-'){state=21;str.push_back(ch);} // δ(0,'-')=21
            else if(ch == '*'){state=25;str.push_back(ch);} // δ(0,'*')=25
            else if(ch == '/'){state=28;str.push_back(ch);} // δ(0,'/')=28
            else if(ch == '{'){state=31;str.push_back(ch);} // δ(0,'{')=31
            else if(ch == '}'){state=32;str.push_back(ch);} // δ(0,'}')=32
            else if(ch == '('){state=33;str.push_back(ch);} // δ(0,'(')=33
            else if(ch == ')'){state=34;str.push_back(ch);} // δ(0,')')=34
            else if(ch == '['){state=35;str.push_back(ch);} // δ(0,'[')=35
            else if(ch == ']'){state=36;str.push_back(ch);} // δ(0,']')=36
            else if(ch == ','){state=37;str.push_back(ch);} // δ(0,',')=37
            else if(ch == ';'){state=38;str.push_back(ch);} // δ(0,';')=38
            else if(ch == 39){state=39;str.push_back(ch);} // δ(0,''')=39
            else if(ch == 34){state=41;str.push_back(ch);} // δ(0,'"')=41
        }
        else if(state == 1){
            if(is_char(ch) || is_digit(ch)){state=1;str.push_back(ch);} // δ(1,字符或数字)=1
            else{
                state = 2;
                ptr_i = ptr_i - 1; // δ(1,其他)=2，终止态
            }
        }
        else if(state == 3){
            if(is_digit(ch)){state=3;str.push_back(ch);} // δ(3,数字=3
            else if(ch == '.'){state=4;str.push_back(ch);} // δ(3,'.')=4
            else{
                state=7;
                ptr_i = ptr_i-1; // δ(3,其他)=7，终止态
            }
        }
        else if(state == 4){
            if(is_digit(ch)){state=5;str.push_back(ch);} // δ(4,数字)=5
            else{
                error = true; //其他情况，编译出错，终止态
            }
        }
        else if(state == 5){
            if(is_digit(ch)){state=5;str.push_back(ch);} // δ(5,数字)=5
            else{
                state=6;
                ptr_i = ptr_i - 1; // δ(5,其他)=6，终止态
            }
        }
        else if(state == 8){
            if(ch == '='){state=9;str.push_back(ch);} // δ(8,'=')=9
            else{
                state=10;
                ptr_i = ptr_i - 1; // δ(8,其他)=10，终止态
            }
        }
        else if(state == 11){
            if(ch == '='){state=12;str.push_back(ch);} // δ(11,'=')=12
            else{
                state=13;
                ptr_i = ptr_i - 1; // δ(11,其他)=13，终止态
            }
        }
        else if(state == 14){
            if(ch == '='){state =15;str.push_back(ch);} // δ(14,'=')=15
            else{
                state=16;
                ptr_i = ptr_i - 1; // δ(14,其他)=16，终止态
            }
        }
        else if(state ==17){
            if(ch == '+'){state=18;str.push_back(ch);} // δ(17,'+')=18
            else if(ch == '='){state=19;str.push_back(ch);} // δ(17,'=')=19
            else{
                state=20;
                ptr_i = ptr_i - 1; // δ(17,其他)=20，终止态
            }
        }
        else if(state == 21){
            if(ch == '-'){state=22;str.push_back(ch);} // δ(21,'-')=22
            else if(ch == '='){state=23;str.push_back(ch);} // δ(21,'=')=23
            else{
                state=24;
                ptr_i = ptr_i - 1; // δ(21,其他)=24，终止态
            }
        }
        else if(state == 25){
            if(ch == '='){state=26;str.push_back(ch);} // δ(25,'=')=26
            else{
                state=27;
                ptr_i = ptr_i - 1; // δ(25,其他)=27，终止态
            }
        }
        else if(state == 28){
            if(ch == '='){state=29;str.push_back(ch);} // δ(28,'=')=29
            else{
                state=30;
                ptr_i = ptr_i - 1; // δ(28,其他)=30，终止态
            }
        }
        else if(state == 39){
            if(ch == 39){state=40;str.push_back(ch);} // δ(39,''')=40，终止态
            else{
                state=39;
                str.push_back(ch); // δ(39,其他)=39
            }
        }
        else if(state == 41){
            if(ch == 34){state=42;str.push_back(ch);} // δ(41,'"')=42，终止态
            else{
                state=41;
                str.push_back(ch); // δ(41,其他)=41
            }
        }
    }

    bool isFstate(int state){
        for(int i : Fstate) {if (state == i) return true;}
        return false;
    }

    void Termination(int &state){ //根据终止状态对词语进行分类
        if(state == 2){ //关键字或标识符
            for (int i = 0; i <= TOKEN_k.size(); ++i) {
                if(i == TOKEN_k.size()){ //不在关键字表中，去标识符表中查询
                    for(int j = 0; j <= TOKEN_i.size(); j++){
                        if(j == TOKEN_i.size()){//表中查不到，将其加到表尾
                            TOKEN_i.push_back(str);
                            ans.emplace_back("ID",TOKEN_i.size()-1);
                            break;
                        }else if(str == TOKEN_i[j]){ //在表中查到
                            ans.emplace_back("ID", j);
                            break;
                        }
                    }
                }else if(str == TOKEN_k[i]){ //在关键字表中
                    ans.emplace_back("KEY",i);
                    break;
                }
            }
            state = 0;
            str.clear();
        }else if(state==9 || state==10 || state==12 || state==13 || state==15 || state==16 ||
                (state>=18 && state<=20) || (state>=22 && state<=24) || state==26 || state==27 ||
                (state>=29 && state<=38)) { //界符
            for(int i = 0; i <= TOKEN_p.size(); ++i){ //在界符表中查询
                if(i == TOKEN_p.size()){ //表中查不到，将其加到表尾
                    TOKEN_p.push_back(str);
                    ans.emplace_back("P",TOKEN_p.size()-1);
                    break;
                }else if(str == TOKEN_p[i]){ //在界符表中
                    ans.emplace_back("P",i);
                    break;
                }
            }
            state = 0;
            str.clear();
        }else if(state == 6 || state == 7){ //数字
            for (int i = 0; i <= TOKEN_c.size(); ++i) {
                if(i == TOKEN_c.size()){ //没在数字表中找到
                    TOKEN_c.push_back(str);
                    ans.emplace_back("NUM",TOKEN_c.size()-1);
                    break;
                }else if(str == TOKEN_c[i]){ //在数字表中
                    ans.emplace_back("NUM", i);
                    break;
                }
            }
            state = 0;
            str.clear();
        }else if(state == 40){ //字符常量
            for (int i = 0; i <= TOKEN_charc.size(); ++i) {
                if(i == TOKEN_charc.size()){ //没在字符常量表中找到
                    TOKEN_charc.push_back(str);
                    ans.emplace_back("CHAR",TOKEN_charc.size()-1);
                    break;
                }else if(str == TOKEN_charc[i]){ //在字符常量表中
                    ans.emplace_back("CHAR", i);
                    break;
                }
            }
            state = 0;
            str.clear();
        }else if(state == 42){ //字符串常量
            for (int i = 0; i <= TOKEN_strc.size(); ++i) {
                if(i == TOKEN_strc.size()){ //没在字符串常量表中找到
                    TOKEN_strc.push_back(str);
                    ans.emplace_back("STRING",TOKEN_strc.size()-1);
                    break;
                }else if(str == TOKEN_strc[i]){ //在字符串常量表中
                    ans.emplace_back("STRING", i);
                    break;
                }
            }
            state = 0;
            str.clear();
        }
    }

    void analyze(){ //词法分析器主体部分
        while(ptr_i < input.length()){
//            cout<<"now state is"<<state<<endl;
            next_state(input[ptr_i]);
            if(error){cout<<"编译出错"<<endl; break;}
            if(isFstate(state)){ //已到达终止状态
                Termination(state);
            }
            ptr_i++;
        }
    }

    void outcome(const char * addr){ //输出结果并保存到文件out.txt中
        ofstream output;
        output.open(addr);
        for(int i = 0; i < ans.size(); i++){ //输出结果
            cout<<ans.size();
            printf("( %s , %d )\n",ans[i].first.c_str(),ans[i].second);
            output <<"( "<<ans[i].first.c_str()<<", "<<ans[i].second<<")"<<endl;
            if(ans[i].first == "KEY"){
                cout<< TOKEN_k[ans[i].second] <<endl;
                output<< TOKEN_k[ans[i].second] <<endl;
            }else if(ans[i].first == "ID"){
                cout<< TOKEN_i[ans[i].second] <<endl;
                output<< TOKEN_i[ans[i].second] <<endl;
            }else if(ans[i].first == "P"){
                cout<< TOKEN_p[ans[i].second] <<endl;
                output<< TOKEN_p[ans[i].second] <<endl;
            }else if(ans[i].first == "NUM"){
                cout<< TOKEN_c[ans[i].second] <<endl;
                output<< TOKEN_c[ans[i].second] <<endl;
            }else if(ans[i].first == "CHAR"){
                cout<< TOKEN_strc[ans[i].second] <<endl;
                output<< TOKEN_strc[ans[i].second] <<endl;
            }else if(ans[i].first == "STRING"){
                cout<< TOKEN_charc[ans[i].second] <<endl;
                output<< TOKEN_charc[ans[i].second] <<endl;
            }
        }
    }

};




int main() {
    LexAnalyzer test;
    const char * addr = "C:/Users/28577/CLionProjects/LexAnalyzer/in.txt";
    test.read_str("../in.txt");
    test.analyze();
    test.outcome("../out.txt");
    system("PAUSE");
    return 0;
}

