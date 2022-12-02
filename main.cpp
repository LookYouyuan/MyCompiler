#include <bits/stdc++.h>
using namespace std;


struct node
{
    string value;
    string type;
};
vector<node> Buffer;

/***-------------------------------------�ʷ�����--------------------------------------------***/
string Key[6] = { "main", "if", "else", "for", "while", "int" };


string Judge( string buffer )
{
    string s = "";
    if ( buffer.length() == 0 )
        return "";
    if ( buffer[0] >= '0' && buffer[0] <= '9' )
    {
        s = " integer     , " + buffer + "  ";
        Buffer.push_back(node{buffer,"number"});
    }
    else
    {
        s = " identifier  , " + buffer + "  ";
        bool flag = false;
        for ( int i = 0; i < 6; i++ )
        {
            if ( buffer == Key[i] )
            {
                s = " keyword     , " + buffer + "  ";
                flag = true;
            }
        }
        if ( flag )
            Buffer.push_back(node{buffer,buffer});
        else Buffer.push_back(node{buffer,"identifier"});
    }
    return s;
}

void Word_analysis(  )
{
    ofstream outfile;
    ifstream infile;
    infile.open("programe.txt");
    outfile.open("main_table.txt");
    string s;
    while ( !infile.eof() )
    {
        infile >> s;
        int l = s.length();
        string buffer;
        for ( int i = 0; i < l; i++ )
        {
            if ( s[i] == '+' || s[i] == '-' || s[i] == '*' || s[i] == '/' ||
                    s[i] == '<' || s[i] == '>' ||    s[i] == '=' || s[i] == '!'  )
            {
                if ( buffer.length() != 0 )
                    outfile << Judge( buffer ) << endl;
                buffer = "";
                if ( i + 1 < l && s[i + 1] == '=' )
                {
                    outfile << " operator    , " + s.substr(i, 2) + "  " << endl;
                    Buffer.push_back(node{s.substr(i, 2), s.substr(i, 2)});
                    i++;
                }
                else
                {
                    outfile << " operator    , " + s.substr(i, 1) + "  " << endl;
                    Buffer.push_back(node{s.substr(i, 1), s.substr(i, 1)});
                }
            }
            else if ( s[i] == '{' || s[i] == '}' ||
                      s[i] == '(' || s[i] == ')' ||
                      s[i] == ',' || s[i] == ';'   )
            {
                if ( buffer.length() != 0 )
                    outfile << Judge( buffer ) << endl;
                buffer = "";
                outfile << " boundary    , " + s.substr(i, 1) + "  " << endl;
                Buffer.push_back(node{s.substr(i, 1), s.substr(i, 1)});
            }
            else
                buffer += s[i];
        }
        if ( buffer.length() != 0 )
            outfile << Judge( buffer ) << endl;
    }
    infile.close();
    outfile.close();
}
/***----------------------------------------�ʷ�����------------------------------------------------***/





/***----------------------------------------�﷨����------------------------------------------------***/
typedef pair<string, string> Pair;
map<Pair, string> Analysis_table;
typedef set<string> Sets;
map<string, Sets> First, Follow, Grammar, Select;      //first, follow
Sets End, NEnd;                //�ռ��������ռ���
string Start;

void Grammar_Map()
{
    string line, suf, pre;
    int cnt = 0;
    freopen("grammar.txt", "r", stdin);
    while(getline(cin, line))
    {
        cnt ++;
        int len = line.size();
        int i = 0;
        while(line[i] != ' ' && i < len)
            i ++;
        pre = line.substr(0, i);
        if(cnt == 1)
            Start = pre;
        i++;
        while(line[i] != ' ' && i < len)
            i ++;
        for(  ; i<len; i++)
        {
            if(line[i] != ' ')
            {
                suf = "";
                while(!(line[i] == '|' && line[i-1] == ' ' && line[i+1] == ' ') && i < len)
                {
                    suf += line[i++];
                }
                while(suf[suf.size()-1] == ' ')
                    suf.erase(suf.size()-1, 1);
                Grammar[pre].insert(suf);
            }
        }
    }
    freopen("CON", "r", stdin);
}

void End_Ch()
{
    fstream in;
    in.open("grammar.txt", ios::in);
    string str;
    while(in >> str)
    {
        if(str == "->") continue;
        if(str[0] == '<' && str[str.size()-1] == '>') NEnd.insert(str);
        else End.insert(str);
    }
    End.insert("#");
    in.close();
}

void First_Sets()
{
    bool flag = true;
    while(flag)
    {
        flag = false;
        map<string, Sets>::iterator Ite;
        for(map<string, Sets>::iterator Ite = Grammar.begin(); Ite != Grammar.end(); Ite++)     //���з��ռ���
        {
            string X = Ite->first;       //���ڷ��ռ���X
            if(Grammar[X].count("@") && !First[X].count("@"))    //����@������FIRST
            {
                First[X].insert("@");
                flag = true;
            }
            Sets::iterator S_ite;
            for(Sets::iterator S_ite = Grammar[X].begin(); S_ite != Grammar[X].end(); S_ite++)     //����s���в���ʽ
            {
                string suf = *S_ite, str = "", last = "";                               //���ڲ���ʽX->Y1Y2Y3...

                int i = 0;
                while(suf[i] != ' ' && i < suf.size()) str += suf[i ++];    //�ҵ�Y1
                if(str == "") continue;

                if(!(str[0] == '<' && str[str.size()-1] == '>'))      //Y1���ռ���
                {
                    if(!First[X].count(str))
                    {
                        First[X].insert(str);
                        flag = true;
                    }
                    if(str != "@") continue;      //���ռ���������@
                }
                else      //Y1�Ƿ��ռ���
                {
                    for(Sets::iterator ii = First[str].begin(); ii != First[str].end(); ii ++)
                    {
                        if(*ii != "@" && !First[X].count(*ii))
                        {
                            First[X].insert(*ii);
                            flag = true;
                        }
                    }
                    if(!First[str].count("@")) continue;
                }

                for( ; i<suf.size(); i++)
                {
                    if(suf[i] != ' ')
                    {
                        last = str;
                        str = "";
                        while(suf[i] != ' ' && i < suf.size()) str += suf[i ++];

                        if(last == "@" || First[last].count("@"))
                        {
                            Sets::iterator S_iteii;
                            for(S_iteii = First[str].begin(); S_iteii != First[str].end(); S_iteii++)
                            {
                                if(*S_iteii != "@" && !First[X].count(*S_iteii))
                                {
                                    First[X].insert(*S_iteii);
                                    flag = true;
                                }
                            }
                        }
                        else break;
                    }
                }

                if(i >= suf.size() && (str == "@" || First[str].count("@")))                           //Y1Y2Y3...Yn��FIRST���ɲ���@
                {
                    if(!First[X].count("@"))
                    {
                        First[X].insert("@");
                        flag = true;
                    }
                }
            }
        }
    }

    fstream out;
    out.open("FIRST.txt", ios::out);       //�������FIRST
    map<string, Sets>::iterator M_ite;
    for( M_ite = First.begin(); M_ite != First.end(); M_ite++ )
    {
        out << "FIRST(" << M_ite->first << "): ";
        Sets::iterator S_iteiii;
        for( S_iteiii = M_ite->second.begin(); S_iteiii != M_ite->second.end(); S_iteiii++ )
            out << " " << *S_iteiii;
        out << endl;
    }
    out.close();
}

void Follow_Sets()
{
    bool flag = 1;
    Follow[Start].insert("#");   //��#����follow(Start)
    string pre, suf, str, last;
    while(flag)
    {
        flag = 0;
        map<string, Sets>::iterator Ite;
        for( Ite = Grammar.begin(); Ite != Grammar.end(); Ite++)
        {
            pre = Ite->first;                                           //����FOLLOW(str)
            Sets::iterator S_Ite;
            for(Sets::iterator S_Ite = Ite->second.begin(); S_Ite != Ite->second.end(); S_Ite++ )
            {
                suf = *S_Ite;                                      //���ڲ���ʽstr->foo��str->ABC
                int len = suf.size(), i = 0;
                str = "";
                while(suf[i] != ' ' && i < len)
                {
                    str += suf[i++];
                }
                if(str == "") continue;

                for( ; i < len; i++)
                {
                    if(suf[i] != ' ')
                    {
                        last = str;
                        str = "";
                        while(suf[i] != ' ' && i < len)
                        {
                            str += suf[i++];
                        }
                        if(last[0] != '<' || last[last.size()-1] != '>') continue;  //B���ս��������

                        if(str[0] != '<' || str[str.size()-1] != '>')    //C���ս��
                        {
                            if(str != "@" && !Follow[last].count(str))
                            {
                                Follow[last].insert(str);
                                flag = 1;
                            }
                            if(str != "@") continue;
                        }
                        else     //C�Ƿ��ս�����Ȱ�first(C) - {@}����
                        {
                            Sets::iterator S_iteii;
                            for( S_iteii = First[str].begin(); S_iteii != First[str].end(); S_iteii++)
                            {
                                if(*S_iteii != "@" && !Follow[last].count(*S_iteii))
                                {
                                    Follow[last].insert(*S_iteii);
                                    flag = 1;
                                }
                            }
                        }

                        if(First[str].count("@") || str == "@")    //C->*@,�Ϳ������
                        {
                            string next = str;
                            int k = i;
                            for( ; k < len; k++)
                            {
                                if(suf[k] != ' ')
                                {
                                    next = "";
                                    while(suf[k] != ' ' && k < len)
                                    {
                                        next += suf[k ++];
                                    }

                                    if(next[0] != '<' || next[next.size()-1] != '>')     //next���ս��
                                    {
                                        if(next != "@" && !Follow[last].count(next))
                                        {
                                            Follow[last].insert(next);
                                            flag = 1;
                                        }
                                        if(next != "@") break;
                                    }
                                    else     //next�Ƿ��ս��
                                    {
                                        Sets::iterator S_iteiii;
                                        for( S_iteiii = First[next].begin(); S_iteiii != First[next].end(); S_iteiii++)
                                        {
                                            if(*S_iteiii != "@" && !Follow[last].count(*S_iteiii))
                                            {
                                                Follow[last].insert(*S_iteiii);
                                                flag = 1;
                                            }
                                        }
                                        if(!First[next].count("@")) break;
                                    }
                                }
                            }
                            if(k >= len && (next == "@" || First[next].count("@")))     //����str->ABC,C->*@,��follow(str) - {@}�ӵ�follow(B)��
                            {
                                Sets::iterator S_iteiv;
                                for( S_iteiv = Follow[pre].begin(); S_iteiv != Follow[pre].end(); S_iteiv++)
                                {
                                    if(!Follow[last].count(*S_iteiv))
                                    {
                                        Follow[last].insert(*S_iteiv);
                                        flag = 1;
                                    }
                                }
                            }
                        }
                    }
                }
                if(i >= len && (str[0] == '<' && str[str.size()-1] == '>'))     //����str->ABC,��follow(str) - {@}�ӵ�follow(C)��
                {
                    Sets::iterator S_itev;
                    for( S_itev = Follow[pre].begin(); S_itev != Follow[pre].end(); S_itev++)
                    {
                        if(!Follow[str].count(*S_itev))
                        {
                            Follow[str].insert(*S_itev);
                            flag = 1;
                        }
                    }
                }
            }
        }
    }

//���FOLLOW��
    fstream out;
    out.open("FOLLOW.txt", ios::out);
    map<string, Sets>::iterator M_Ite;
    for( M_Ite = Follow.begin(); M_Ite != Follow.end(); M_Ite++)
    {
        out << "FOLLOW(" << M_Ite->first << "): ";
        Sets::iterator S_it;
        for( S_it = M_Ite->second.begin(); S_it != M_Ite->second.end(); S_it++ )
            out << " " << *S_it;
        out << endl;
    }
    out.close();
}

void Select_Sets()
{
    string pre, suf, str, all;
    Sets buf;
    map<string, Sets>::iterator Ite;
    for(map<string, Sets>::iterator Ite = Grammar.begin(); Ite != Grammar.end(); Ite++)
    {
        pre = Ite->first;
        Sets::iterator S_iteI;                                        //����str
        for(Sets::iterator S_iteI = Ite->second.begin(); S_iteI != Ite->second.end(); S_iteI++)
        {
            suf = *S_iteI;
            all = pre + " " + suf;            //����select(str->foo)

            buf.clear();
            int len = suf.size();
            for(int i=0; i<len; i++)
            {
                if(suf[i] != ' ')
                {
                    str = "";
                    while(suf[i] != ' ' && i < len)
                    {
                        str += suf[i ++];
                    }
                    if(str[0] != '<' || str[str.size()-1] != '>')   //s���ս��
                    {
                        if(str != "@")
                        {
                            buf.insert(str);
                            break;
                        }
                    }
                    else
                    {
                        Sets::iterator S_iteII;
                        for( S_iteII = First[str].begin(); S_iteII != First[str].end(); S_iteII++)
                        {
                            if(*S_iteII != "@") buf.insert(*S_iteII);
                        }
                        if(!First[str].count("@")) break;
                    }
                }
            }
            if(str == "@" || First[str].count("@"))
            {
                set_union(Select[all].begin(), Select[all].end(), Follow[pre].begin(), Follow[pre].end(), inserter(Select[all], Select[all].begin()));
                set_union(Select[all].begin(), Select[all].end(), buf.begin(), buf.end(), inserter(Select[all], Select[all].begin()));
            }
            else set_union(buf.begin(), buf.end(), Select[all].begin(), Select[all].end(), inserter(Select[all], Select[all].begin()));
        }
    }

    //���SELECT��
    fstream out;
    out.open("SELECT.txt", ios::out);
    map<string, Sets>::iterator M_Ite;
    for( M_Ite = Select.begin(); M_Ite != Select.end(); M_Ite++)
    {
        out << "SELECT(" << M_Ite->first << "): ";
        Sets::iterator S_ite;
        for(Sets::iterator S_ite = M_Ite->second.begin(); S_ite != M_Ite->second.end(); S_ite++)
            out << " " << *S_ite;
        out << endl;
    }
    out.close();
}

void Analysis_Table()
{
    End_Ch();
    Analysis_table.clear();
    string str, pre;
    map<string, Sets>::iterator Ite;
    for( Ite = Select.begin(); Ite != Select.end(); Ite++ )
    {
        int i = 0;
        str = pre = "";
        while(Ite->first[i] != ' ' && i < Ite->first.size())
            str += Ite->first[i++];
        pre = Ite->first.substr(i+1, Ite->first.size() - i - 1);

        Sets::iterator S_ite;
        for( S_ite = Ite->second.begin(); S_ite != Ite->second.end(); S_ite++)
            Analysis_table[Pair(str, *S_ite)] = pre;
    }
    fstream out;
    out.open("analysis_table.txt", ios::out);

    out<<"        ";
    Sets::iterator S_itei;
    for( S_itei = End.begin(); S_itei != End.end(); S_itei++)
        out << setw(8) << *S_itei;
    out << endl;
    Sets::iterator S_iteii;
    for( S_iteii = NEnd.begin(); S_iteii != NEnd.end(); S_iteii++)
    {
        out << setw(8) << *S_iteii;
        Sets::iterator S_iteiii;
        for( S_iteiii = End.begin(); S_iteiii != End.end(); S_iteiii++)
            out<<setw(8)<<(Analysis_table[Pair(*S_iteii, *S_iteiii)] == "" ? "*" : Analysis_table[Pair(*S_iteii, *S_iteiii)]);
        out<<endl;
    }
    out.close();
}

bool Analysis(ofstream &out)          //��������
{
    vector<string> Gram;
    string str, pre;
    Gram.push_back("#");          //Ԥ����
    Gram.push_back(Start);
    Buffer.push_back(node{"#","#"});
    out<<"------------------------------------------------------------------------------\n";
    out<<"  ���� |    �ķ�����ջ��    |     ������ջ��     |   ���ò���ʽ��ƥ��         \n";
    out<<"------------------------------------------------------------------------------\n";
    int step = 1, pos = 0;
    while(!Gram.empty() && pos < Buffer.size())
    {
        out<<setw(6)<<(step ++)<<setw(18)<<Gram.back()<<setw(19)<<Buffer[pos].type;
        if(Gram.back() == "#" && Buffer[pos].type == "#")
        {
            out<<"    "<<"����"<<endl;
            return true;
        }
        if(Gram.back() == Buffer[pos].type)
        {
            out<<"    "<<"��"<<Buffer[pos].type<<"��"<<"ƥ��"<<endl;
            Gram.pop_back();
            pos ++;
        }
        else
        {
            str = Analysis_table[Pair(Gram.back(), Buffer[pos].type)];
            if(str == "")
                return false;
            out << "    " << Gram.back() << " -> " << str << endl;
            if(str == "@")
                str = "";
            Gram.pop_back();
            int i = str.size()-1;
            for( ; i >= 0; i--)
            {
                if(str[i] != ' ')
                {
                    pre = "";
                    while(str[i] != ' ' && i >= 0) pre += str[i--];
                    reverse(pre.rbegin(), pre.rend());
                    Gram.push_back(pre);
                }
            }
        }
    }
    return false;
}

bool grammar_analysis()
{
    Grammar_Map();
    First_Sets();
    Follow_Sets();
    Select_Sets();
    Analysis_Table();

//�����������
    ofstream out;
    out.open("grammar_analysis.txt", ios::out);
    bool flag = Analysis(out);
    out<<(flag ? "\n\n-->LL(1)�Ϸ�����\n\n" : "\n\n-->LL(1)�Ƿ�����\n\n");
    out.close();
    return flag;
}




/*******************************���롪���м��������*************************************/
int cnt;
node now, last;

struct Four_node    //��Ԫʽ�ṹ��
{
    string op;
    string R1, R2;
    string jump;
};
vector<Four_node> four_node;   //��Ԫʽ

int cnt_temp;

map<string, int> pri;   //���ȼ�����

void Init_pri()     //���ȼ�����
{
    pri["("] = 7;
    pri["*"] = pri["/"]  = 6;
    pri["+"] = pri["-"] = 5;
    pri[">"] = pri["<"] = pri[">="] = pri["<="] = 4;
    pri["!="] = pri["=="] = 3;
    pri["="] = 2;
    pri[")"] = pri["@"] = 1;
}

void Get_FourNode()     //��ȡ��Ԫʽ
{
    last = now;
    now = Buffer[cnt++];
}

bool Expect(string expect)    //ƥ�亯��
{
    if(now.type == expect || now.value == expect)
    {
        Get_FourNode();
        return true;
    }
    else return false;
}

string Int_toString(int x)     //int to string
{
    stringstream ss;
    ss << x;
    return ss.str();
}

string Caculate(string stop)    //���ʽ
{
    int             bra = 0;
    string          op;
    string          arg1, arg2, t = "";
    string          v;
    stack<string>   num;
    stack<string>   opr;
    opr.push("@");
    while(true)
    {
        v = now.value;
        //������ʽ�Ƿ����
        if(v == "(")    bra--;
        if(v == ")")    bra++;
        if(bra > 0 || Expect(stop))  v = "@";

        //���ʽ����
        if(v != "@" && (now.type == "identifier" || now.type == "number"))  //��Ϊ��������ʱ
        {
            num.push(v);
            if(Expect("identifier"))         t = "";
            else if(Expect("number")) t = "";
        }
        else  //��Ϊ�����ʱ
        {
            op = opr.top();
            while(pri[v] <= pri[op] && !(op == "(" && v != ")"))
            {
                if(op == "@" && v == "@")
                {
                    return num.top();
                }
                opr.pop();

                if(op == "(" && v == ")")
                {
                    break;
                }
                else  //��Ϊ˫Ŀ�����
                {
                    arg2 = num.top();
                    num.pop();
                    arg1 = num.top();
                    num.pop();

                    four_node.push_back(Four_node{op, arg1, arg2, "t" + Int_toString(cnt_temp ++)});
                }

                num.push("t" + Int_toString(cnt_temp - 1));
                op = opr.top();
            }
            if(v != ")" && v != "@")    opr.push(v);
            Expect(v);
        }
    }
}

void Equal(node buf, string stop)    //��ֵ���
{
    Expect("=");
    string tt = Caculate(stop);
    four_node.push_back(Four_node{"=", tt, "_", buf.value});
}

bool Function_body()                       //������
{
    while(true)
    {
        if(Expect("int"))                  //�������
        {
            if(Expect("identifier"))
            {
                Expect(";");
            }
        }
        else if(Expect("identifier"))      //��ֵ���
        {
            Equal(last, ";");
        }
        else if(Expect("if") && Expect("("))
        {
            string tt = Caculate(")");
            Expect("{");
            four_node.push_back(Four_node{"J=", tt, "1", "(" + Int_toString(four_node.size()+2) + ")"});
            four_node.push_back(Four_node{"J", "_", "_", ""});
            int ad = four_node.size() - 1;
            Function_body();
            if(Expect("else") && Expect("{"))
            {
                four_node.push_back(Four_node{"J", "_", "_", ""});
                int sd = four_node.size() - 1;
                four_node[ad].jump += "(" + Int_toString(four_node.size()) + ")";
                Function_body();
                four_node[sd].jump += "(" + Int_toString(four_node.size()) + ")";
            }
            else four_node[ad].jump += "(" + Int_toString(four_node.size()) + ")";
        }
        else if(Expect("while") && Expect("("))
        {
            string tt = Caculate(")");
            Expect(")");
            Expect("{");
            four_node.push_back(Four_node{"J=", tt, "1", "(" + Int_toString(four_node.size()+2) + ")"});
            four_node.push_back(Four_node{"J", "_", "_", ""});
            int ad = four_node.size() - 1;
            Function_body();
            four_node.push_back(Four_node{"J", "_", "_", "(" + Int_toString(ad - 1) + ")"});
            four_node[ad].jump += "(" + Int_toString(four_node.size()) + ")";
        }
        else if(Expect("for") && (Expect("(")))
        {
            Expect("identifier");
            Equal(last, ";");

            string tt = Caculate(";");
            four_node.push_back(Four_node{"J=", tt, "1", "(" + Int_toString(four_node.size()+3) + ")"});

            int ft = four_node.size() + 2;
            Expect("identifier");
            Equal(last, ")");

            Function_body();
            four_node.push_back(Four_node{"J", "_", "_", "(" + Int_toString(ft) + ")"});

        }
        else
        {
            Expect("}");
            return true;
        }
    }
    return false;
}

void FourNode()      //�����Ԫʽ
{
    fstream out;
    out.open("four_node.txt", ios::out);
    int n = four_node.size();
    out<<"                     "<<endl;
    out<<"---Four_expression---"<<endl;
    out<<"                     "<<endl;
    for(int i=0; i<n; i++)
    {
        out<<"("<<i<<")"<<"  ( "<<four_node[i].op<<" , "<<four_node[i].R1<<" , "<<four_node[i].R2<<" , "<<four_node[i].jump<<" )"<<endl;
    }
}

bool translation()
{
    Init_pri();
    Get_FourNode();
    if(Expect("main") && Expect("(") && Expect(")") && Expect("{"))
    {
        if(Function_body())
        {
            FourNode();
            return true;
        }
    }
    FourNode();
    return false;
}

void Look1()
{
    ifstream infile("main_table.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look2()
{
    ifstream infile("FIRST.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look3()
{
    ifstream infile("FOLLOW.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look4()
{
    ifstream infile("SELECT.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look5()
{
    ifstream infile("analysis_table.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look6()
{
    ifstream infile("grammar_analysis.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look7()
{
    ifstream infile("four_node.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Lookx()
{
    ifstream infile("programe.txt");
    string line;
    if ( infile )
        while ( getline(infile, line) )
            cout << line << endl;
    else
        cout << "û�и��ļ���Ϣ" << endl;
}

void Look(  )
{

    while (true)
    {
        cout << "------------------------�鿴���---------------------------" << endl;
        cout << "-----------------------------------------------------------" << endl;
        cout << "----------                         *.�鿴��Ҫ�����ĳ���----" << endl;
        cout << "----------                         1.�鿴�ʷ�������� -----" << endl;
        cout << "----------                         2.�鿴FIRST����    -----" << endl;
        cout << "----------                         3.�鿴FOLLOW����   -----" << endl;
        cout << "----------                         4.�鿴SELECT����   -----" << endl;
        cout << "----------                         5.�鿴Ԥ�������   -----" << endl;
        cout << "----------                         6.�鿴�﷨������� -----" << endl;
        cout << "----------                         7.�鿴�м���� ��Ԫʽ---" << endl;
        cout << "----------                         0.�˳�             -----" << endl;
        cout << "-----------------------------------------------------------" << endl;
        cout << "-----------------------------------------------------------" << endl;

        char look[1000];
        scanf("%s", look);
        if ( ( look[0] < '0' || look[0] > '7' ) && look[0] != '*'  )
        {
            printf("%c\n", look[0]);
            cout << "Input Error!" << endl;
            continue;
        }
        switch ( look[0] )
        {
        case '*':
            Lookx();
            break;
        case '0':
            exit(0);
            break;
        case '1':
            Look1();
            break;
        case '2':
            Look2();
            break;
        case '3':
            Look3();
            break;
        case '4':
            Look4();
            break;
        case '5':
            Look5();
            break;
        case '6':
            Look6();
            break;
        case '7':
            Look7();
            break;
        }
    }


}

void All_Analysis( )
{
    Word_analysis();
    cout << "�ʷ������ɹ���" << endl;
    if ( grammar_analysis() )
        cout << "�﷨�����ɹ���" << endl;
    else
        cout << "�﷨����ʧ�ܣ�" << endl;
    if ( translation() )
        cout << "�������ʽ����ɹ���" << endl;
    else
        cout << "�������ʽ����ʧ�ܣ�" << endl;
}

int main()
{
    All_Analysis();
    Look();
    return 0;
}
