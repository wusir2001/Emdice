/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //Ӧ��AppID����Ϣ������ȷ��д�������Q�����޷�����

#include "QTool.h"

#include <map>
#include <set>
#include <string>

#include <fstream>
#include <io.h>

#include <random>
#include <ctime>

using namespace std;

int ac = -1; //AuthCode ���ÿ�Q�ķ���ʱ��Ҫ�õ�
bool enabled = false;

//[CQ:at,qq=QQ��]

const string help = "����EmDiceʹ��˵������\n������������������\n��bot��һ����Դ��Ŀ��Դ��������github����EmDiceBot\n\n[һ�㹦��]\n-������������������������		.r2d6\n-��Ĭ������������			.rd\n-��������˽�Ľ����			.rh 1d20��\n-��Ͷ�� coc7�����ԣ�˽�Ľ������	.coc 5��\n-�����õ�ǰ�ǳ�			.nn ���ǳ�\n-������Ĭ��������������		.d 20\n-��Ͷ������������d100������		.rb\n-��Ͷ�����ͷ�����d100������		.rp\n-����ݼ��ܼ춨������������		.rc\n-���鿴�������ơ���������		.jrrp\n-���鿴ʹ�ð�����˽�Ľ����		.help	\n\n[���ع���]\n.fku\n.Em\n.Emnz��δʵװ��\n.Emsb��δʵװ��\n\n����.���á����棬���ϸ����ִ�Сд��Ŀǰ��bot��ʱֻ����������ʹ�á�";

struct jrrp
{
	int32_t jrrp;
	int32_t day;
	int32_t year;
};

map<int64_t, string> name;
map<int64_t, int32_t> defaultDice;
map<int64_t, jrrp> jrrpmap;
set<int64_t> enabledGroup;
set<int64_t> admin;

random_device rd;
mt19937 mt(rd());

CQTool Turn;
string ret;
tm local;
time_t now;

const string coc7[] = { "����", "����", "����", "���", "��־", "����", "����", "����", "����" };

//int32_t record[120];

/*
void test()
{
	memset(record, 0, sizeof record);
	uniform_int_distribution<> u(1, 100);
	for (int i = 1; i <= 1000000; ++i)
		++record[u(mt)];
}
*/
/*map<int64_t, string>::iterator plus1(map<int64_t, string>::iterator it)
{
	return ++it;
}*/
void save()
{
	ofstream oname("./app/dice/name.ini");
	ofstream odice("./app/dice/dice.ini");
	ofstream oadmin("./app/dice/admin.ini");
	ofstream ojrrp("./app/dice/jrrp.ini");
	for (map<int64_t, string>::iterator it = name.begin(); it != name.end(); ++it)
		oname << it->first << " " << it->second << endl;
	oname << "-";
	for (map<int64_t, int32_t>::iterator it = defaultDice.begin(); it != defaultDice.end(); ++it)
		odice << it->first << " " << it->second << endl;
	odice << "-1";
	for (set<int64_t>::iterator it = admin.begin(); it != admin.end(); ++it)
		oadmin << *it << endl;
	oadmin << "-1";
	for (map<int64_t, jrrp>::iterator it = jrrpmap.begin(); it != jrrpmap.end(); ++it)
		ojrrp << it->first << " " << it->second.jrrp << " " << it->second.year << " " << it->second.day << endl;
	ojrrp << "-1";
}

void load()
{
	if (_access("./app/dice/name.ini", 0) != -1)
	{
		string instr;
		ifstream iname("./app/dice/name.ini");
		while (getline(iname, instr))
		{
			if (instr[0] == '-')
				break;
			int32_t i = 1;
			int64_t QQnum = instr[0] - '0';
			for (; isdigit(instr[i]); ++i)
				QQnum = QQnum * 10 + instr[i] - '0';
			++i;
			string nick = instr.substr(i, instr.length());
			name[QQnum] = nick;
		}
	}
	else
	{
		ofstream oname("./app/dice/name.ini");
		oname << "-";
	}

	if (_access("./app/dice/dice.ini", 0) != -1)
	{
		ifstream idice("./app/dice/dice.ini");
		while (1)
		{
			int64_t QQnum;
			int32_t dicenum;
			idice >> QQnum;
			if (QQnum != -1)
			{
				idice >> dicenum;
				defaultDice[QQnum] = dicenum;
			}
			else
				break;
		}
	}
	else
	{
		ofstream odice("./app/dice/dice.ini");
		odice << -1;
	}

	if (_access("./app/dice/admin.ini", 0) != -1)
	{
		ifstream iadmin("./app/dice/admin.ini");
		while (1)
		{
			int64_t QQnum;
			iadmin >> QQnum;
			if (QQnum != -1)
				admin.insert(QQnum);
			else
				break;
		}
	}
	else
	{
		ofstream oadmin("./app/dice/admin.ini");
		oadmin << -1;
	}

	if (_access("./app/dice/jrrp.ini", 0) != -1)
	{
		ifstream ijrrp("./app/dice/jrrp.ini");
		while (1)
		{
			int64_t QQnum;
			jrrp current;
			ijrrp >> QQnum;
			if (QQnum != -1)
			{
				ijrrp >> current.jrrp >> current.year >> current.day;
				jrrpmap[QQnum] = current;
			}
			else
				break;
		}
	}
	else
	{
		ofstream ojrrp("./app/dice/jrrp.ini");
		ojrrp << -1;
	}
}

void updatejrrp(int64_t fromQQ)
{
	uniform_int_distribution<> u(1, 100);
	now = time(NULL);
	localtime_s(&local, &now);
	jrrp current;
	current.jrrp = u(mt);
	current.day = local.tm_yday, current.year = local.tm_year;
	if (!jrrpmap.count(fromQQ))
		jrrpmap[fromQQ] = current;
	else if (!(current.day == jrrpmap[fromQQ].day&&current.year == jrrpmap[fromQQ].year))
		jrrpmap[fromQQ] = current;
}

void atme(int64_t fromQQ)
{
	char tmp[12];
	_i64toa_s(fromQQ, tmp, 12, 10);
	ret = "*[CQ:at,qq=";
	ret += tmp;
	ret += "] ";
}

const char* nnDelete(int64_t fromQQ)
{
	atme(fromQQ);
	if (!name.count(fromQQ))
		ret += "��δ�����ǳƣ��޷������";
	else
	{
		ret += "��������Լ����ǳƣ�";
		name.erase(fromQQ);
	}
	return ret.data();
}

const char* nnAdd(int64_t fromQQ, string newname)
{
	atme(fromQQ);
	ret += "�����ǳ��ǣ�";
	ret += newname;
	name[fromQQ] = newname;
	return ret.data();
}

const char* fku(int64_t fromQQ)
{
	atme(fromQQ);
	ret += "FKU!";
	return ret.data();
}

void atname(int64_t fromQQ)
{
	ret = "* ";
	if (name.count(fromQQ))
		ret += name[fromQQ];
	else
	{
		CQ_TYPE_QQ tmp;
		Turn.GetStrangerInfo(ac, fromQQ, tmp);
		ret += tmp.nick;
	}
}

void atname(int64_t fromQQ, int64_t fromGroup)
{
	ret = "* ";
	if (name.count(fromQQ))
		ret += name[fromQQ];
	else
	{
		CQ_Type_GroupMember tmp;
		Turn.GetGroupMemberInfo(ac, fromGroup, fromQQ, tmp);
		ret += tmp.nick;
	}
}

const char* getjrrp(int64_t fromQQ)
{
	atname(fromQQ);
	ret += " ���������ָ���� ";
	char tmp[7];
	_i64toa_s(jrrpmap[fromQQ].jrrp, tmp, 7, 10);
	ret += tmp;
	ret += "%��";
	for (int i = 1; i <= min(jrrpmap[fromQQ].jrrp,100); ++i)
		ret += '|';
	return ret.data();
}

const char* rDice(int64_t fromQQ, int32_t range, int32_t num)
{
	char tmp[12];
	int32_t res = 0;
	range = (range == 0) ? ((defaultDice.count(fromQQ)) ? defaultDice[fromQQ] : 100) : range;
	num = (num == 0) ? 1 : num;
	uniform_int_distribution<> u(1, range);
	//ʹ��u(mt)��ȡ�����
	atname(fromQQ);
	ret += " Ͷ�� ";
	_i64toa_s(num, tmp, 12, 10);
	ret += tmp;
	ret += "d";
	_i64toa_s(range, tmp, 12, 10);
	ret += tmp;
	ret += "��";
	for (int32_t i = 1; i <= num; ++i)
	{
		int32_t cut = u(mt);
		res += cut;
		_i64toa_s(cut, tmp, 12, 10);
		ret += tmp;
		if (i != num)
			ret += "��";
	}
	if (num != 1)
	{
		ret += " = ";
		_i64toa_s(res, tmp, 12, 10);
		ret += tmp;
	}
	return ret.data();
}

/* 
* ����Ӧ�õ�ApiVer��Appid������󽫲������
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* ����Ӧ��AuthCode����Q��ȡӦ����Ϣ��������ܸ�Ӧ�ã���������������������AuthCode��
* ��Ҫ�ڱ��������������κδ��룬���ⷢ���쳣���������ִ�г�ʼ����������Startup�¼���ִ�У�Type=1001����
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 ��Q����
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q������ִ��һ�Σ���������ִ��Ӧ�ó�ʼ�����롣
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventStartup, 0)() {
	load();
	return 0;
}


/*
* Type=1002 ��Q�˳�
* ���۱�Ӧ���Ƿ����ã������������ڿ�Q�˳�ǰִ��һ�Σ���������ִ�в���رմ��롣
* ������������Ϻ󣬿�Q���ܿ�رգ��벻Ҫ��ͨ���̵߳ȷ�ʽִ���������롣
*/
CQEVENT(int32_t, __eventExit, 0)() {
	save();
	return 0;
}

/*
* Type=1003 Ӧ���ѱ�����
* ��Ӧ�ñ����ú󣬽��յ����¼���
* �����Q����ʱӦ���ѱ����ã�����_eventStartup(Type=1001,��Q����)�����ú󣬱�����Ҳ��������һ�Ρ�
* ��Ǳ�Ҫ����������������ش��ڡ���������Ӳ˵������û��ֶ��򿪴��ڣ�
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	load();
	enabled = true;
	return 0;
}


/*
* Type=1004 Ӧ�ý���ͣ��
* ��Ӧ�ñ�ͣ��ǰ�����յ����¼���
* �����Q����ʱӦ���ѱ�ͣ�ã��򱾺���*����*�����á�
* ���۱�Ӧ���Ƿ����ã���Q�ر�ǰ��������*����*�����á�
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	save();
	enabled = false;
	return 0;
}


/*
* Type=21 ˽����Ϣ
* subType �����ͣ�11/���Ժ��� 1/��������״̬ 2/����Ⱥ 3/����������
*/
CQEVENT(int32_t, __eventPrivateMsg, 24)(int32_t subType, int32_t msgId, int64_t fromQQ, const char *msg, int32_t font) {
	if (msg[0] == '.' || (msg[0] == -95 && msg[1] == -93))
	{
		int32_t sp = 2;
		if (msg[0] == '.')
			sp = 1;
		if (msg[sp] == 'E'&&msg[sp+1] == 'm'&&msg[sp+2] == '\0')
			CQ_sendPrivateMsg(ac, fromQQ, "*EmDice Alive");
		if (msg[sp] == 's'&&msg[sp + 1] == 'a'&&msg[sp + 2] == 'v'&&msg[sp + 3] == 'e'&&msg[sp + 4] == '\0'&&admin.count(fromQQ))
		{
			save();
			CQ_sendPrivateMsg(ac, fromQQ, "*�������");
		}
		if (msg[sp] == 'l'&&msg[sp + 1] == 'o'&&msg[sp + 2] == 'a'&&msg[sp + 3] == 'd'&&msg[sp + 4] == '\0'&&admin.count(fromQQ))
		{
			load();
			CQ_sendPrivateMsg(ac, fromQQ, "*��ȡ���");
		}
	}
	//���Ҫ�ظ���Ϣ������ÿ�Q�������ͣ��������� return EVENT_BLOCK - �ضϱ�����Ϣ�����ټ�������  ע�⣺Ӧ�����ȼ�����Ϊ"���"(10000)ʱ������ʹ�ñ�����ֵ
	//������ظ���Ϣ������֮���Ӧ��/�������������� return EVENT_IGNORE - ���Ա�����Ϣ
	return EVENT_IGNORE;
}


/*
* Type=2 Ⱥ��Ϣ
*/
CQEVENT(int32_t, __eventGroupMsg, 36)(int32_t subType, int32_t msgId, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (msg[0] == '.' || (msg[0] == -95 && msg[1] == -93))
	{
		int sp = 2;
		if (msg[0] == '.')
			sp = 1;
		if (msg[sp] == 'E'&&msg[sp + 1] == 'm'&&msg[sp + 2] == '\0')
			CQ_sendGroupMsg(ac, fromGroup, "*EmDice Alive");
	}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=4 ��������Ϣ
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	if (msg[0] == '.' || (msg[0] == -95 && msg[1] == -93))//�ж��Ƿ�Ϊ����
	{
		int32_t sp = 2;
		if (msg[0] == '.')
			sp = 1;

		if (msg[sp] == 'f'&&msg[sp + 1] == 'k'&&msg[sp + 2] == 'u'&&msg[sp + 3] == '\0')//��fku
		{
/*			test();
			for (int i = 1; i <= 100; ++i)
			{
				char tmp[8];
				_i64toa_s(record[i], tmp, 8, 10);
				CQ_sendPrivateMsg(ac, fromQQ, tmp);
			}*/
			CQ_sendDiscussMsg(ac, fromDiscuss, fku(fromQQ));
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'E'&&msg[sp + 1] == 'm'&&msg[sp + 2] == '\0')//��Em
		{
			CQ_sendDiscussMsg(ac, fromDiscuss, "*EmDice Alive");
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'j'&&msg[sp + 1] == 'r'&&msg[sp + 2] == 'r'&&msg[sp + 3] == 'p'&&msg[sp + 4] == '\0')
		{
			updatejrrp(fromQQ);
			CQ_sendDiscussMsg(ac, fromDiscuss, getjrrp(fromQQ));
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'n'&&msg[sp + 1] == 'n')//��nnϵ��
		{
			sp += 2;
			while (msg[sp] == ' ')
				++sp;
			if (msg[sp] == '\0')
				CQ_sendDiscussMsg(ac, fromDiscuss, nnDelete(fromQQ));
			else
			{
				string tmp;
				while (msg[sp] != '\0'&&msg[sp] != '\n')
					tmp += msg[sp], ++sp;
				CQ_sendDiscussMsg(ac, fromDiscuss, nnAdd(fromQQ, tmp));
			}
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'd')//��dĬ������
		{
			++sp;
			atname(fromQQ);
			if (msg[sp] == '\0')
			{
				defaultDice[fromQQ] = 100;
				ret += " �ָ�Ĭ������100";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			int32_t endp = sp;
			while (isdigit(msg[endp]))
				++endp;
			if (msg[endp] != '\0')
				return EVENT_IGNORE;
			if (endp - sp > 3)
			{
				ret += " ����ʧ�ܣ�������Χ����Χ��1 - 100��";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			int32_t diceNum = msg[sp] - '0';
			for (++sp; sp < endp; ++sp)
				diceNum = diceNum * 10 + msg[sp] - '0';
			if (diceNum > 100 || diceNum == 0)
			{
				ret += " ����ʧ�ܣ�������Χ����Χ��1 - 100��";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			defaultDice[fromQQ] = diceNum;
			char tmp[6];
			_i64toa_s(diceNum, tmp, 6, 10);
			ret += " ����Ĭ������Ϊ";
			ret += tmp;
			CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'c'&&msg[sp + 1] == 'o'&&msg[sp + 2] == 'c')//coc 7
		{
			sp += 3;
			int32_t times = 0;
			char tmp[6];
			atname(fromQQ);
			uniform_int_distribution<> u(1, 6);
			ret += " Ͷ��COC 7�� ���ԣ�\n";
			while (msg[sp] == ' ')
				++sp;
			if (isdigit(msg[sp]))
				times = msg[sp] - '0', ++sp;
			if (isdigit(msg[sp]))
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*������Χ����Χ��1 - 5");
				return EVENT_BLOCK;
			}
			if (times > 5)
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*������Χ����Χ��1 - 5");
				return EVENT_BLOCK;
			}
			if (times == 0)
				++times;
			for (int32_t i = 1; i <= times; ++i)//���� ���� ���� ��� ��־ ���� ���� ���� ����
				//���� ���� ���� ��� ��־ ����Ϊ3d6�ٳ���5������ ���� ����Ϊ2d6+6�ٳ���5
			{
				int32_t quality[10] = { (u(mt) + u(mt) + u(mt)) * 5, (u(mt) + u(mt) + u(mt)) * 5, (u(mt) + u(mt) + u(mt)) * 5, (u(mt) + u(mt) + u(mt)) * 5, (u(mt) + u(mt) + u(mt)) * 5, (u(mt) + u(mt) + u(mt)) * 5, (u(mt) + u(mt) + 6) * 5, (u(mt) + u(mt) + 6) * 5, (u(mt) + u(mt) + 6) * 5, (u(mt) + u(mt) + u(mt)) * 5 };
				quality[9] = quality[0] + quality[1] + quality[2] + quality[3] + quality[4] + quality[5] + quality[6] + quality[7];
				for (int32_t i = 0; i < 9; ++i)
				{
					ret += coc7[i];
					ret += " ";
					_i64toa_s(quality[i], tmp, 6, 10);
					ret += tmp;
					if (i != 8)
						ret += ' ';
				}
				ret += "\n�ܺͣ�";
				_i64toa_s(quality[9], tmp, 6, 10);
				ret += tmp;
				ret += " + ";
				_i64toa_s(quality[8], tmp, 6, 10);
				ret += tmp;
				if (i != times)
					ret += "\n";
			}
			string toQQ = ret;
			atme(fromQQ);
			ret += " �ѷ�����˽�ģ��뵽˽�Ĳ鿴��";
			CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
			CQ_sendPrivateMsg(ac, fromQQ, toQQ.data());
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'r')//������
		{
			++sp;
			if ((msg[sp] == 'c'))//rc���ܼ춨
			{
				char tmp[6];
				++sp;
				atname(fromQQ);
				uniform_int_distribution<> u(1, 100);
				int32_t result = u(mt), can=0;
				_i64toa_s(result, tmp, 6, 10);
				ret += " ���м��ܼ춨��";
				ret += tmp;
				ret += " ";
				while (msg[sp] == ' ')
					++sp;
				while (!isdigit(msg[sp]))
					ret += msg[sp], ++sp;
				while (msg[sp] == ' ')
					++sp;
				while (isdigit(msg[sp]))
					can = can * 10 + msg[sp] - '0', ++sp;
				if (result == 1)
					ret += " ��ɹ���";
				else if (result == 100)
					ret += "��ʧ�ܡ�";
				else if (result <= can / 5)
					ret += " ���ѳɹ���";
				else if (result <= can / 2)
					ret += " ���ѳɹ���";
				else if (result <= can)
					ret += " ����ɹ���";
				else if (result >= 96 && can < 50)
					ret += " ��ʧ�ܡ�";
				else ret += " δͨ����";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			if ((msg[sp] == 'b' || msg[sp] == 'p') && msg[sp + 1] == '\0')//b=���� p=�ͷ�
			{
				char tmp[6];
				atname(fromQQ);
				uniform_int_distribution<> u(1, 100);
				int32_t d100 = u(mt), d10 = u(mt) % 10, m10 = d100 % 10, m100 = (d100 - m10) / 10;
				++d10;
				ret += " Ͷ��";
				ret += (msg[sp] == 'b') ? "����" : "�ͷ�";
				ret += "����";
				_i64toa_s(d100, tmp, 6, 10);
				ret += tmp;
				ret += "��";
				_i64toa_s(d10, tmp, 6, 10);
				ret += tmp;
				if (m10 != 0 && d10 == 10)
					d10 = 0;
				d100 = ((msg[sp] == 'b') ? min(m100, d10) : max(m100, d10)) * 10 + m10;
				ret += " = ";
				_i64toa_s(d100, tmp, 6, 10);
				ret += tmp;
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			int32_t num = 0;
			while (isdigit(msg[sp]))
				num = num * 10 + msg[sp] - '0', ++sp;
			if (msg[sp] != 'd')
				return EVENT_IGNORE;
			++sp;
			int32_t range = 0;
			while (isdigit(msg[sp]))
				range = range * 10 + msg[sp] - '0', ++sp;
			if (msg[sp] != 'h'&&msg[sp] != '\0')
				return EVENT_IGNORE;
			if (range > 100)
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*������Χ����Χ��1 - 100");
				return EVENT_BLOCK;
			}
			if (num > 10)
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*����̫���ˣ����ʮ��Ŷ��");
				return EVENT_BLOCK;
			}
			if (msg[sp] != 'h')
				CQ_sendDiscussMsg(ac, fromDiscuss, rDice(fromQQ, range, num));
			else
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "��������ѷ�����˽��\0");
				CQ_sendPrivateMsg(ac, fromQQ, rDice(fromQQ, range, num));
			}
			return EVENT_BLOCK;
		}
		if (msg[sp] == 'h'&&msg[sp + 1] == 'e'&&msg[sp + 2] == 'l'&&msg[sp + 3] == 'p'&&msg[sp + 4] == '\0')
		{
			atme(fromQQ);
			ret += "�ѷ�����˽�ģ��뵽˽�Ĳ鿴��";
			CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
			CQ_sendPrivateMsg(ac, fromQQ, help.data());
		}
	}
	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=101 Ⱥ�¼�-����Ա�䶯
* subType �����ͣ�1/��ȡ������Ա 2/�����ù���Ա
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=102 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/ȺԱ�뿪 2/ȺԱ���� 3/�Լ�(����¼��)����
* fromQQ ������QQ(��subTypeΪ2��3ʱ����)
* beingOperateQQ ������QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=103 Ⱥ�¼�-Ⱥ��Ա����
* subType �����ͣ�1/����Ա��ͬ�� 2/����Ա����
* fromQQ ������QQ(������ԱQQ)
* beingOperateQQ ������QQ(����Ⱥ��QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=201 �����¼�-���������
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=301 ����-�������
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_BLOCK; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}


/*
* Type=302 ����-Ⱥ���
* subType �����ͣ�1/����������Ⱥ 2/�Լ�(����¼��)������Ⱥ
* msg ����
* responseFlag ������ʶ(����������)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else
	if (subType == 2) {
		CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
		return EVENT_BLOCK;
	}

	return EVENT_IGNORE; //���ڷ���ֵ˵��, ����_eventPrivateMsg������
}

/*
* �˵������� .json �ļ������ò˵���Ŀ��������
* �����ʹ�ò˵������� .json ���˴�ɾ�����ò˵�

CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "����menuA�����������봰�ڣ����߽�������������", "" ,0);
	return 0;
}
*/

/*
todolist:
rd����
sc
help
welcome
Ⱥ��˽��
*/