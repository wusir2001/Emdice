/*
* CoolQ Demo for VC++ 
* Api Version 9
* Written by Coxxs & Thanks for the help of orzFly
*/

#include "stdafx.h"
#include "string"
#include "cqp.h"
#include "appmain.h" //应用AppID等信息，请正确填写，否则酷Q可能无法加载

#include "QTool.h"

#include <map>
#include <set>
#include <string>

#include <fstream>
#include <io.h>

#include <random>
#include <ctime>

using namespace std;

int ac = -1; //AuthCode 调用酷Q的方法时需要用到
bool enabled = false;

//[CQ:at,qq=QQ号]

const string help = "——EmDice使用说明——\n嘤嘤嘤嘤嘤嘤嘤嘤嘤\n本bot是一个开源项目，源码请自行github搜索EmDiceBot\n\n[一般功能]\n-　掷骰　　　　　　　　　		.r2d6\n-　默认掷骰　　　			.rd\n-　暗骰（私聊结果）			.rh 1d20　\n-　投掷 coc7版属性（私聊结果）　	.coc 5　\n-　设置当前昵称			.nn 新昵称\n-　设置默认掷骰面数　　		.d 20\n-　投掷带奖励骰的d100　　　		.rb\n-　投掷带惩罚骰的d100　　　		.rp\n-　便捷技能检定　　　　　　		.rc\n-　查看今日运势　　　　　		.jrrp\n-　查看使用帮助（私聊结果）		.help	\n\n[神秘功能]\n.fku\n.Em\n.Emnz（未实装）\n.Emsb（未实装）\n\n以上.可用。代替，且严格区分大小写。目前本bot暂时只能在讨论组使用。";

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

const string coc7[] = { "力量", "敏捷", "体质", "外表", "意志", "智力", "体型", "教育", "幸运" };

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
		ret += "您未设置昵称，无法清除！";
	else
	{
		ret += "您已清除自己的昵称！";
		name.erase(fromQQ);
	}
	return ret.data();
}

const char* nnAdd(int64_t fromQQ, string newname)
{
	atme(fromQQ);
	ret += "的新昵称是：";
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
	ret += " 今天的运势指数是 ";
	char tmp[7];
	_i64toa_s(jrrpmap[fromQQ].jrrp, tmp, 7, 10);
	ret += tmp;
	ret += "%！";
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
	//使用u(mt)调取随机数
	atname(fromQQ);
	ret += " 投掷 ";
	_i64toa_s(num, tmp, 12, 10);
	ret += tmp;
	ret += "d";
	_i64toa_s(range, tmp, 12, 10);
	ret += tmp;
	ret += "：";
	for (int32_t i = 1; i <= num; ++i)
	{
		int32_t cut = u(mt);
		res += cut;
		_i64toa_s(cut, tmp, 12, 10);
		ret += tmp;
		if (i != num)
			ret += "、";
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
* 返回应用的ApiVer、Appid，打包后将不会调用
*/
CQEVENT(const char*, AppInfo, 0)() {
	return CQAPPINFO;
}


/* 
* 接收应用AuthCode，酷Q读取应用信息后，如果接受该应用，将会调用这个函数并传递AuthCode。
* 不要在本函数处理其他任何代码，以免发生异常情况。如需执行初始化代码请在Startup事件中执行（Type=1001）。
*/
CQEVENT(int32_t, Initialize, 4)(int32_t AuthCode) {
	ac = AuthCode;
	return 0;
}


/*
* Type=1001 酷Q启动
* 无论本应用是否被启用，本函数都会在酷Q启动后执行一次，请在这里执行应用初始化代码。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventStartup, 0)() {
	load();
	return 0;
}


/*
* Type=1002 酷Q退出
* 无论本应用是否被启用，本函数都会在酷Q退出前执行一次，请在这里执行插件关闭代码。
* 本函数调用完毕后，酷Q将很快关闭，请不要再通过线程等方式执行其他代码。
*/
CQEVENT(int32_t, __eventExit, 0)() {
	save();
	return 0;
}

/*
* Type=1003 应用已被启用
* 当应用被启用后，将收到此事件。
* 如果酷Q载入时应用已被启用，则在_eventStartup(Type=1001,酷Q启动)被调用后，本函数也将被调用一次。
* 如非必要，不建议在这里加载窗口。（可以添加菜单，让用户手动打开窗口）
*/
CQEVENT(int32_t, __eventEnable, 0)() {
	load();
	enabled = true;
	return 0;
}


/*
* Type=1004 应用将被停用
* 当应用被停用前，将收到此事件。
* 如果酷Q载入时应用已被停用，则本函数*不会*被调用。
* 无论本应用是否被启用，酷Q关闭前本函数都*不会*被调用。
*/
CQEVENT(int32_t, __eventDisable, 0)() {
	save();
	enabled = false;
	return 0;
}


/*
* Type=21 私聊消息
* subType 子类型，11/来自好友 1/来自在线状态 2/来自群 3/来自讨论组
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
			CQ_sendPrivateMsg(ac, fromQQ, "*保存完毕");
		}
		if (msg[sp] == 'l'&&msg[sp + 1] == 'o'&&msg[sp + 2] == 'a'&&msg[sp + 3] == 'd'&&msg[sp + 4] == '\0'&&admin.count(fromQQ))
		{
			load();
			CQ_sendPrivateMsg(ac, fromQQ, "*读取完毕");
		}
	}
	//如果要回复消息，请调用酷Q方法发送，并且这里 return EVENT_BLOCK - 截断本条消息，不再继续处理  注意：应用优先级设置为"最高"(10000)时，不得使用本返回值
	//如果不回复消息，交由之后的应用/过滤器处理，这里 return EVENT_IGNORE - 忽略本条消息
	return EVENT_IGNORE;
}


/*
* Type=2 群消息
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
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=4 讨论组消息
*/
CQEVENT(int32_t, __eventDiscussMsg, 32)(int32_t subType, int32_t msgId, int64_t fromDiscuss, int64_t fromQQ, const char *msg, int32_t font) {
	if (msg[0] == '.' || (msg[0] == -95 && msg[1] == -93))//判断是否为命令
	{
		int32_t sp = 2;
		if (msg[0] == '.')
			sp = 1;

		if (msg[sp] == 'f'&&msg[sp + 1] == 'k'&&msg[sp + 2] == 'u'&&msg[sp + 3] == '\0')//。fku
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

		if (msg[sp] == 'E'&&msg[sp + 1] == 'm'&&msg[sp + 2] == '\0')//。Em
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

		if (msg[sp] == 'n'&&msg[sp + 1] == 'n')//。nn系列
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

		if (msg[sp] == 'd')//。d默认骰子
		{
			++sp;
			atname(fromQQ);
			if (msg[sp] == '\0')
			{
				defaultDice[fromQQ] = 100;
				ret += " 恢复默认设置100";
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
				ret += " 设置失败，超出范围！范围：1 - 100！";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			int32_t diceNum = msg[sp] - '0';
			for (++sp; sp < endp; ++sp)
				diceNum = diceNum * 10 + msg[sp] - '0';
			if (diceNum > 100 || diceNum == 0)
			{
				ret += " 设置失败，超出范围！范围：1 - 100！";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			defaultDice[fromQQ] = diceNum;
			char tmp[6];
			_i64toa_s(diceNum, tmp, 6, 10);
			ret += " 设置默认骰子为";
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
			ret += " 投掷COC 7版 属性：\n";
			while (msg[sp] == ' ')
				++sp;
			if (isdigit(msg[sp]))
				times = msg[sp] - '0', ++sp;
			if (isdigit(msg[sp]))
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*超出范围！范围：1 - 5");
				return EVENT_BLOCK;
			}
			if (times > 5)
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*超出范围！范围：1 - 5");
				return EVENT_BLOCK;
			}
			if (times == 0)
				++times;
			for (int32_t i = 1; i <= times; ++i)//力量 敏捷 体质 外表 意志 智力 体型 教育 幸运
				//力量 敏捷 体质 外表 意志 幸运为3d6再乘以5，智力 体型 教育为2d6+6再乘以5
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
				ret += "\n总和：";
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
			ret += " 已发送至私聊，请到私聊查看！";
			CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
			CQ_sendPrivateMsg(ac, fromQQ, toQQ.data());
			return EVENT_BLOCK;
		}

		if (msg[sp] == 'r')//扔骰子
		{
			++sp;
			if ((msg[sp] == 'c'))//rc技能检定
			{
				char tmp[6];
				++sp;
				atname(fromQQ);
				uniform_int_distribution<> u(1, 100);
				int32_t result = u(mt), can=0;
				_i64toa_s(result, tmp, 6, 10);
				ret += " 进行技能检定：";
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
					ret += " 大成功！";
				else if (result == 100)
					ret += "大失败。";
				else if (result <= can / 5)
					ret += " 极难成功！";
				else if (result <= can / 2)
					ret += " 困难成功！";
				else if (result <= can)
					ret += " 常规成功！";
				else if (result >= 96 && can < 50)
					ret += " 大失败。";
				else ret += " 未通过。";
				CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
				return EVENT_BLOCK;
			}
			if ((msg[sp] == 'b' || msg[sp] == 'p') && msg[sp + 1] == '\0')//b=奖励 p=惩罚
			{
				char tmp[6];
				atname(fromQQ);
				uniform_int_distribution<> u(1, 100);
				int32_t d100 = u(mt), d10 = u(mt) % 10, m10 = d100 % 10, m100 = (d100 - m10) / 10;
				++d10;
				ret += " 投掷";
				ret += (msg[sp] == 'b') ? "奖励" : "惩罚";
				ret += "骰：";
				_i64toa_s(d100, tmp, 6, 10);
				ret += tmp;
				ret += "、";
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
				CQ_sendDiscussMsg(ac, fromDiscuss, "*超出范围！范围：1 - 100");
				return EVENT_BLOCK;
			}
			if (num > 10)
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "*骰子太多了！最多十个哦？");
				return EVENT_BLOCK;
			}
			if (msg[sp] != 'h')
				CQ_sendDiscussMsg(ac, fromDiscuss, rDice(fromQQ, range, num));
			else
			{
				CQ_sendDiscussMsg(ac, fromDiscuss, "暗骰结果已发送至私聊\0");
				CQ_sendPrivateMsg(ac, fromQQ, rDice(fromQQ, range, num));
			}
			return EVENT_BLOCK;
		}
		if (msg[sp] == 'h'&&msg[sp + 1] == 'e'&&msg[sp + 2] == 'l'&&msg[sp + 3] == 'p'&&msg[sp + 4] == '\0')
		{
			atme(fromQQ);
			ret += "已发送至私聊，请到私聊查看！";
			CQ_sendDiscussMsg(ac, fromDiscuss, ret.data());
			CQ_sendPrivateMsg(ac, fromQQ, help.data());
		}
	}
	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=101 群事件-管理员变动
* subType 子类型，1/被取消管理员 2/被设置管理员
*/
CQEVENT(int32_t, __eventSystem_GroupAdmin, 24)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=102 群事件-群成员减少
* subType 子类型，1/群员离开 2/群员被踢 3/自己(即登录号)被踢
* fromQQ 操作者QQ(仅subType为2、3时存在)
* beingOperateQQ 被操作QQ
*/
CQEVENT(int32_t, __eventSystem_GroupMemberDecrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=103 群事件-群成员增加
* subType 子类型，1/管理员已同意 2/管理员邀请
* fromQQ 操作者QQ(即管理员QQ)
* beingOperateQQ 被操作QQ(即加群的QQ)
*/
CQEVENT(int32_t, __eventSystem_GroupMemberIncrease, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, int64_t beingOperateQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=201 好友事件-好友已添加
*/
CQEVENT(int32_t, __eventFriend_Add, 16)(int32_t subType, int32_t sendTime, int64_t fromQQ) {

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=301 请求-好友添加
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddFriend, 24)(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, const char *responseFlag) {

	CQ_setFriendAddRequest(ac, responseFlag, REQUEST_ALLOW, "");

	return EVENT_BLOCK; //关于返回值说明, 见“_eventPrivateMsg”函数
}


/*
* Type=302 请求-群添加
* subType 子类型，1/他人申请入群 2/自己(即登录号)受邀入群
* msg 附言
* responseFlag 反馈标识(处理请求用)
*/
CQEVENT(int32_t, __eventRequest_AddGroup, 32)(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *msg, const char *responseFlag) {

	//if (subType == 1) {
	//	CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPADD, REQUEST_ALLOW, "");
	//} else
	if (subType == 2) {
		CQ_setGroupAddRequestV2(ac, responseFlag, REQUEST_GROUPINVITE, REQUEST_ALLOW, "");
		return EVENT_BLOCK;
	}

	return EVENT_IGNORE; //关于返回值说明, 见“_eventPrivateMsg”函数
}

/*
* 菜单，可在 .json 文件中设置菜单数目、函数名
* 如果不使用菜单，请在 .json 及此处删除无用菜单

CQEVENT(int32_t, __menuA, 0)() {
	MessageBoxA(NULL, "这是menuA，在这里载入窗口，或者进行其他工作。", "" ,0);
	return 0;
}
*/

/*
todolist:
rd完善
sc
help
welcome
群和私聊
*/