#include<cstdio>
#include<cstring>
#include<string>
#include<iostream>
#include<map>
using namespace std;

class run_system {
public:
	int Pos, address, Rdest, Rsrc1, Rsrc2, Src1, Src2;
	long long Result;
	bool r_flag;
	run_system() :
		Pos(0), Result(0), address(0), r_flag(false), 
		Rdest(-1), Rsrc1(-1), Rsrc2(-1), Src1(0), Src2(0) {}
	run_system & operator=(const run_system &other) {
		Pos = other.Pos;
		Result = other.Result;
		address = other.address;
		Rdest = other.Rdest;
		Rsrc1 = other.Rsrc1;
		Rsrc2 = other.Rsrc2;
		Src1 = other.Src1;
		Src2 = other.Src2;
		r_flag = other.r_flag;
	}
	void clean() {
		r_flag = false;
		Rdest = Rsrc1 = Rsrc2 = -1;
		Pos = Result = address = Src1 = Src2 = 0;
	}
} tmp[6];
struct opera {
	int num, val;
	bool flag;
	string op, data[3];
} order[10000];
int e_pos = 0, r_pos = 0, f[35] = {0}, PC_register;
int judge[35] = {0};
unsigned char data[4000001] = {0};
map<string, int> e_Label, r_Label, _register, _order;
FILE *pF;

void init() {
	_register["$0"] = _register["$zero"] = 0;
	_register["$1"] = _register["$at"] = 1;
	_register["$2"] = _register["$v0"] = 2;
	_register["$3"] = _register["$v1"] = 3;
	_register["$4"] = _register["$a0"] = 4;
	_register["$5"] = _register["$a1"] = 5;
	_register["$6"] = _register["$a2"] = 6;
	_register["$7"] = _register["$a3"] = 7;
	_register["$8"] = _register["$t0"] = 8;
	_register["$9"] = _register["$t1"] = 9;
	_register["$10"] = _register["$t2"] = 10;
	_register["$11"] = _register["$t3"] = 11;
	_register["$12"] = _register["$t4"] = 12;
	_register["$13"] = _register["$t5"] = 13;
	_register["$14"] = _register["$t6"] = 14;
	_register["$15"] = _register["$t7"] = 15;
	_register["$16"] = _register["$s0"] = 16;
	_register["$17"] = _register["$s1"] = 17;
	_register["$18"] = _register["$s2"] = 18;
	_register["$19"] = _register["$s3"] = 19;
	_register["$20"] = _register["$s4"] = 20;
	_register["$21"] = _register["$s5"] = 21;
	_register["$22"] = _register["$s6"] = 22;
	_register["$23"] = _register["$s7"] = 23;
	_register["$24"] = _register["$t8"] = 24;
	_register["$25"] = _register["$t9"] = 25;
	_register["$26"] = _register["$k0"] = 26;
	_register["$27"] = _register["$k1"] = 27;
	_register["$28"] = _register["$gp"] = 28;
	_register["$29"] = _register["$sp"] = 29;
	_register["$30"] = _register["$s8"] = _register["$fp"] = 30;
	_register["$31"] = _register["$ra"] = 31;
	_register["$lo"] = 32;
	_register["$hi"] = 33;
	_order["add"] = 1;
	_order["addu"] = 2;
	_order["addiu"] = 3;
	_order["sub"] = 4;
	_order["subu"] = 5;
	_order["mul"] = 6;
	_order["mulu"] = 7;
	_order["div"] = 8;
	_order["divu"] = 9;
	_order["xor"] = 10;
	_order["xoru"] = 11;
	_order["neg"] = 12;
	_order["negu"] = 13;
	_order["rem"] = 14;
	_order["remu"] = 15;
	_order["li"] = 16;
	_order["seq"] = 17;
	_order["sge"] = 18;
	_order["sgt"] = 19;
	_order["sle"] = 20;
	_order["slt"] = 21;
	_order["sne"] = 22;
	_order["b"] = 23;
	_order["beq"] = 24;
	_order["bne"] = 25;
	_order["bge"] = 26;
	_order["ble"] = 27;
	_order["bgt"] = 28;
	_order["blt"] = 29;
	_order["beqz"] = 30;
	_order["bnez"] = 31;
	_order["bgez"] = 32;
	_order["blez"] = 33;
	_order["bgtz"] = 34;
	_order["bltz"] = 35;
	_order["j"] = 36;
	_order["jr"] = 37;
	_order["jal"] = 38;
	_order["jalr"] = 39;
	_order["la"] = 40;
	_order["lb"] = 41;
	_order["lh"] = 42;
	_order["lw"] = 43;
	_order["sb"] = 44;
	_order["sh"] = 45;
	_order["sw"] = 46;
	_order["move"] = 47;
	_order["mfhi"] = 48;
	_order["mflo"] = 49;
	_order["nop"] = 50;
	_order["syscall"] = 51;
	f[29] = 4000000;
}

void GetIn() {
	int len;
	char s[100];
	bool flag = true;
	while(fscanf(pF, "%s", s) != EOF) {
		if(!strcmp(s, ".data")) {
			flag = true;
			continue;
		}
		if(!strcmp(s, ".text")) {
			flag = false;
			continue;
		}
		len = strlen(s);
		if(s[len - 1] == ':') {
			s[len - 1] = 0;
			string tmp = s;
			if(flag) e_Label[tmp] = e_pos;
			else r_Label[tmp] = r_pos;
			continue;
		}
		if(s[0] == '.') {
			if(s[1] == 's') {
				int tmp;
				fscanf(pF, "%d", &tmp);
				e_pos += tmp;
				continue;
			}
			if(s[1] == 'b' || s[1] == 'h' || s[1] == 'w') {
				char tmp[12];
				int now = 0, sum = 0, pos = 0, exp = 1;
				while(fscanf(pF, "%s", tmp)) {
					now = pos = 0;
					sum = strlen(tmp);
					if(tmp[0] == '-') {
						exp = -1;
						pos = 1;
					}
					while(pos < sum && tmp[pos] != ',') {
						now *= 10;
						now += (tmp[pos] - '0') * exp;
						++pos;
					}
					if(s[1] == 'h' || s[1] == 'w') {
						if(s[1] == 'w') {
							data[e_pos++] = (now >> 24) % (1 << 8);
							data[e_pos++] = (now >> 16) % (1 << 8);
						}
						data[e_pos++] = (now >> 8) % (1 << 8);
					}
					data[e_pos++] = now % (1 << 8);
					if(pos == sum) break;
				}
			} else {
				if(s[len - 1] == 'n') {
					int tmp;
					fscanf(pF, "%d", &tmp);
					e_pos += ((1 << tmp) - e_pos % ( 1 << tmp));
				} else {
					int sum = 0, pos = 0;
					char tmp[100];
					fgets(tmp, 100, pF);
					sum = strlen(tmp);
					pos = 0;
					while(tmp[pos++] != '\"');
					while(pos < sum - 2) {
						data[e_pos++] = tmp[pos++];
						if(tmp[pos - 1] == '\\') { 
							switch (tmp[pos++]) {
								case '0' : data[e_pos - 1] = 0; break;
								case 'a' : data[e_pos - 1] = 7; break;
								case 'b' : data[e_pos - 1] = 8; break;
								case 't' : data[e_pos - 1] = 9; break;
								case 'n' : data[e_pos - 1] = 10; break;
								case 'v' : data[e_pos - 1] = 11; break;
								case 'f' : data[e_pos - 1] = 12; break;
								case 'r' : data[e_pos - 1] = 13; break;
								case '?' : data[e_pos - 1] = 63; break;
								case '\"' : data[e_pos - 1] = 34; break;
								case '\'' : data[e_pos - 1] = 39; break;
								case '\\' : data[e_pos - 1] = 92; break;
							}
						}
					}
					if(len == 7) e_pos++;
				}
			}
		} else {
			int sum = 0, pos = 0, now = 0, tmp = 1;
			bool flag = false;
			order[r_pos].op = s;
			order[r_pos].num = 0;
			order[r_pos].val = 0;
			order[r_pos].flag = false;
			if(strcmp(s, "syscall") && strcmp(s, "nop")) flag = true;
			while(flag) {
				fscanf(pF, "%s", s);
				sum = strlen(s);
				pos = now = 0;
				tmp = 1;
				if(s[sum - 1] == ')') s[--sum] = 0;
				if(s[sum - 1] == ',') s[--sum] = 0;
				else flag = false;
				if(s[0] == '-') {
					tmp = -1;
					pos = 1;
				}
				while(pos < sum) {
					if(s[pos] < '0' || s[pos] > '9') break;
					now *= 10;
					now += (s[pos++] - '0') * tmp;
				}
				if(pos != sum) {
					order[r_pos].data[order[r_pos].num++] = s;
				}
				if(pos) {
					order[r_pos].val = now;
					order[r_pos].flag = true;
				}
				if(pos && pos != sum) {
					order[r_pos].data[order[r_pos].num - 1].erase(0, pos + 1);
				}
			}
			++r_pos;
		}
	}
}

bool Data_Preparation() {
	int pos = tmp[2].Pos, now = _order[order[pos].op];
	if(now == 16) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Src1 = order[pos].val;
		return true;
	}
	if(now == 12) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register[order[pos].data[1]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		return true;
	}
	if(now == 13) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register[order[pos].data[1]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		if(tmp[2].Src1 < 0) tmp[2].Src1 = -tmp[2].Src1;
		return true;
	}
	if(now == 1 || now == 4 || now == 10 || now == 14 || 
		(now <= 22 && now >= 17)) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register[order[pos].data[1]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		if(order[pos].flag) tmp[2].Src2 = order[pos].val;
		else {
			tmp[2].Rsrc2 = _register[order[pos].data[2]];
			if(judge[tmp[2].Rsrc2]) return false;
			tmp[2].Src2 = f[tmp[2].Rsrc2];
		}
		return true;
	}
	if(now == 2 || now == 3 || now == 5 || now == 11 || now == 15) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register[order[pos].data[1]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		if(tmp[2].Src1 < 0) tmp[2].Src1 = -tmp[2].Src1;
		if(order[pos].flag) {
			tmp[2].Src2 = order[pos].val;
			if(tmp[2].Src2 < 0) tmp[2].Src2 = -tmp[2].Src2;
		}
		else {
			tmp[2].Rsrc2 = _register[order[pos].data[2]];
			if(judge[tmp[2].Rsrc2]) return false;
			tmp[2].Src2 = f[tmp[2].Rsrc2];
			if(tmp[2].Src2 < 0) tmp[2].Src2 = -tmp[2].Src2;
		}
		return true;
	}
	if(now == 6 || now == 8) {
		if(order[pos].num + order[pos].flag == 3) {
			tmp[2].Rdest = _register[order[pos].data[0]];
			tmp[2].Rsrc1 = _register[order[pos].data[1]];
			if(judge[tmp[2].Rsrc1]) return false;
			tmp[2].Src1 = f[tmp[2].Rsrc1];
			if(order[pos].flag) tmp[2].Src2 = order[pos].val;
			else {
				tmp[2].Rsrc2 = _register[order[pos].data[2]];
				if(judge[tmp[2].Rsrc2]) return false;
				tmp[2].Src2 = f[tmp[2].Rsrc2];
			}
		} else {
			tmp[2].Rdest = _register["$lo"];
			tmp[2].Rsrc1 = _register[order[pos].data[0]];
			if(judge[tmp[2].Rsrc1]) return false;
			tmp[2].Src1 = f[tmp[2].Rsrc1];
			if(order[pos].flag) tmp[2].Src2 = order[pos].val;
			else {
				tmp[2].Rsrc2 = _register[order[pos].data[1]];
				if(judge[tmp[2].Rsrc2]) return false;
				tmp[2].Src2 = f[tmp[2].Rsrc2];
			}
		}
		return true;
	}
	if(now == 7 || now == 9) {
		if(order[pos].num + order[pos].flag == 3) {
			tmp[2].Rdest = _register[order[pos].data[0]];
			tmp[2].Rsrc1 = _register[order[pos].data[1]];
			if(judge[tmp[2].Rsrc1]) return false;
			tmp[2].Src1 = f[tmp[2].Rsrc1];
			if(tmp[2].Src1 < 0) tmp[2].Src1 = -tmp[2].Src1;
			if(order[pos].flag) {
				tmp[2].Src2 = order[pos].val;
				if(tmp[2].Src2 < 0) tmp[2].Src2 = -tmp[2].Src2;
			}
			else {
				tmp[2].Rsrc2 = _register[order[pos].data[2]];
				if(judge[tmp[2].Rsrc2]) return false;
				tmp[2].Src2 = f[tmp[2].Rsrc2];
				if(tmp[2].Src2 < 0) tmp[2].Src2 = -tmp[2].Src2;
			}
		} else {
			tmp[2].Rdest = _register["$lo"];
			tmp[2].Rsrc1 = _register[order[pos].data[0]];
			if(judge[tmp[2].Rsrc1]) return false;
			tmp[2].Src1 = f[tmp[2].Rsrc1];
			if(tmp[2].Src1 < 0) tmp[2].Src1 = -tmp[2].Src1;
			if(order[pos].flag) {
				tmp[2].Src2 = order[pos].val;
				if(tmp[2].Src2 < 0) tmp[2].Src2 = -tmp[2].Src2;
			}
			else {
				tmp[2].Rsrc2 = _register[order[pos].data[1]];
				if(judge[tmp[2].Rsrc2]) return false;
				tmp[2].Src2 = f[tmp[2].Rsrc2];
				if(tmp[2].Src2 < 0) tmp[2].Src2 = -tmp[2].Src2;
			}
		}
		return true;
	}
	if(now == 38 || now == 39) {
		tmp[2].Rdest = _register["$ra"];
	}
	if(now == 23 || now == 36 || now == 38) {
		tmp[2].address = r_Label[order[pos].data[0]];
		return true;
	}
	if(now == 37 || now == 39) {
		tmp[2].Rsrc1 = _register[order[pos].data[0]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].address = f[tmp[2].Rsrc1];
		return true;
	}
	if(now <= 35 && now >= 30) {
		tmp[2].Rsrc1 = _register[order[pos].data[0]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		tmp[2].address = r_Label[order[pos].data[1]];
		return true;
	}
	if(now <= 29 && now >= 24) {
		tmp[2].Rsrc1 = _register[order[pos].data[0]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		if(order[pos].flag) tmp[2].Src2 = order[pos].val;
		else {
			tmp[2].Rsrc2 = _register[order[pos].data[1]];
			if(judge[tmp[2].Rsrc2]) return false;
			tmp[2].Src2 = f[tmp[2].Rsrc2];
		}
		tmp[2].address = r_Label[order[pos].data[order[pos].num - 1]];
		return true;
	}
	if(now <= 43 && now >= 40) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		if(order[pos].flag) {
			tmp[2].Rsrc1 = _register[order[pos].data[1]];
			if(judge[tmp[2].Rsrc1]) return false;
			tmp[2].address = f[tmp[2].Rsrc1] + order[pos].val;
		} else tmp[2].address = e_Label[order[pos].data[1]];
		return true;
	}
	if(now <= 46 && now >= 44) {
		tmp[2].Rsrc1 = _register[order[pos].data[0]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		if(order[pos].flag) {
			tmp[2].Rsrc2 = _register[order[pos].data[1]];
			if(judge[tmp[2].Rsrc2]) return false;
			tmp[2].address = f[tmp[2].Rsrc2] + order[pos].val;
		} else tmp[2].address = e_Label[order[pos].data[1]];
		return true;
	}
	if(now == 47) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register[order[pos].data[1]];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		return true;
	}
	if(now == 48) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register["$hi"];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		return true;
	}
	if(now == 49) {
		tmp[2].Rdest = _register[order[pos].data[0]];
		tmp[2].Rsrc1 = _register["$lo"];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		return true;
	}
	if(now == 51) {
		tmp[2].Rsrc1 = _register["$v0"];
		if(judge[tmp[2].Rsrc1]) return false;
		tmp[2].Src1 = f[tmp[2].Rsrc1];
		if(tmp[2].Src1 == 5 || tmp[2].Src1 == 9) {
			tmp[2].Rdest = _register["$v0"];
		}
		if(tmp[2].Src1 == 5 || tmp[2].Src1 == 10) return true;
		tmp[2].Rsrc2 = _register["$a0"];
		if(judge[tmp[2].Rsrc2]) return false;
		tmp[2].Src2 = f[tmp[2].Rsrc2];
		return true;
	}
	return true;
}

void Execution() {
	int now = tmp[3].Pos;
	long long x, y;
	switch (_order[order[now].op]) {
		case 1  :
			tmp[3].Result = tmp[3].Src1 + tmp[3].Src2;	
			break;
		case 2  : 
			tmp[3].Result = tmp[3].Src1 + tmp[3].Src2;
			break;
		case 3  :
			tmp[3].Result = tmp[3].Src1 + tmp[3].Src2;
			break;
		case 4  : 
			tmp[3].Result = tmp[3].Src1 - tmp[3].Src2;
			break;
		case 5  :
			tmp[3].Result = tmp[3].Src1 - tmp[3].Src2;
			break;
		case 6  : 
			x = tmp[3].Src1;
			y = tmp[3].Src2;
			tmp[3].Result = x * y;
			break;
		case 7  : 
			x = tmp[3].Src1;
			y = tmp[3].Src2;
			tmp[3].Result = x * y;
			break;
		case 8  : 
			x = tmp[3].Src1 / tmp[3].Src2;
			y = tmp[3].Src1 % tmp[3].Src2;
			tmp[3].Result = x | (y << 32);
			break;
		case 9  : 
			x = tmp[3].Src1 / tmp[3].Src2;
			y = tmp[3].Src1 % tmp[3].Src2;
			tmp[3].Result = x | (y << 32);
			break;
		case 10 : 
			tmp[3].Result = tmp[3].Src1 ^ tmp[3].Src2;
			break;
		case 11 :
			tmp[3].Result = tmp[3].Src1 ^ tmp[3].Src2;
			break;
		case 12 :
			tmp[3].Result = -tmp[3].Src1;
			break;
		case 13 :
			tmp[3].Result = -tmp[3].Src1;
			break;
		case 14 :
			tmp[3].Result = tmp[3].Src1 % tmp[3].Src2;
			break;
		case 15 :
			tmp[3].Result = tmp[3].Src1 % tmp[3].Src2;
			break;
		case 16 :
			tmp[3].Result = tmp[3].Src1;
			break;
		case 17 :
			tmp[3].Result = (tmp[3].Src1 == tmp[3].Src2);
			break;
		case 18 :
			tmp[3].Result = (tmp[3].Src1 >= tmp[3].Src2);
			break;
		case 19 : 
			tmp[3].Result = (tmp[3].Src1 > tmp[3].Src2);
			break;
		case 20 :
			tmp[3].Result = (tmp[3].Src1 <= tmp[3].Src2);
			break;
		case 21 :
			tmp[3].Result = (tmp[3].Src1 < tmp[3].Src2);
			break;
		case 22 :
			tmp[3].Result = (tmp[3].Src1 != tmp[3].Src2);
			break;
		case 23 :
			tmp[3].Result = 1;
			break;
		case 24 :
			tmp[3].Result = (tmp[3].Src1 == tmp[3].Src2);
			break;
		case 25 :
			tmp[3].Result = (tmp[3].Src1 != tmp[3].Src2);
		 	break;
		case 26 :
			tmp[3].Result = (tmp[3].Src1 >= tmp[3].Src2);
			break;
		case 27 :
			tmp[3].Result = (tmp[3].Src1 <= tmp[3].Src2);
			break;
		case 28 :
			tmp[3].Result = (tmp[3].Src1 > tmp[3].Src2);
			break;
		case 29 :
			tmp[3].Result = (tmp[3].Src1 < tmp[3].Src2);
			break;
		case 30 :
			tmp[3].Result = (tmp[3].Src1 == 0);
			break;
		case 31 :
			tmp[3].Result = (tmp[3].Src1 != 0);
			break;
		case 32 :
			tmp[3].Result = (tmp[3].Src1 >= 0);
			break;
		case 33 :
			tmp[3].Result = (tmp[3].Src1 <= 0);
			break;
		case 34 :
			tmp[3].Result = (tmp[3].Src1 > 0);
			break;
		case 35 :
			tmp[3].Result = (tmp[3].Src1 < 0);
			break;
		case 36 :
			tmp[3].Result = 1;
			break;
		case 37 :
			tmp[3].Result = 1;
			break;
		case 38 :
			tmp[3].Result = now + 1;
			break;
		case 39 :
			tmp[3].Result = now + 1;
			break;
		case 40:
			tmp[3].Result = tmp[3].address;
			break;
		case 47 :
			tmp[3].Result = tmp[3].Src1;
			break;
		case 48 :
			tmp[3].Result = tmp[3].Src1;
			break;
		case 49 :
			tmp[3].Result = tmp[3].Src1;
			break;
	}
}

void Memory_Access() {
	int now = tmp[4].Pos, a, b, c, d;
	char c_tmp[1000];
	switch (_order[order[now].op]) {
		case 41 :
			a = data[tmp[4].address];
			tmp[4].Result = a;
			break;
		case 42 :
			a = data[tmp[4].address] << 8;
			b = data[tmp[4].address + 1];
			tmp[4].Result = a | b;
			break;
		case 43 :
			a = data[tmp[4].address] << 24;
			b = data[tmp[4].address + 1] << 16;
			c = data[tmp[4].address + 2] << 8;
			d = data[tmp[4].address + 3];
			tmp[4].Result = a | b | c | d;
			break;
		case 44 :
			data[tmp[4].address] = tmp[4].Src1 % (1 << 8);
			break;
		case 45 :
			data[tmp[4].address] = (tmp[4].Src1 >> 8) % (1 << 8);
			data[tmp[4].address + 1] = tmp[4].Src1 % (1 << 8);
			break;
		case 46 :
			data[tmp[4].address] = (tmp[4].Src1 >> 24) % (1 << 8);
			data[tmp[4].address + 1] = (tmp[4].Src1 >> 16) % (1 << 8);
			data[tmp[4].address + 2] = (tmp[4].Src1 >> 8) % (1 << 8);
			data[tmp[4].address + 3] = tmp[4].Src1 % (1 << 8);
			break;
		case 51 :
			switch (tmp[4].Src1) {
				case 1 : 
					printf("%d\n", tmp[4].Src2);
					break;
				case 4 :
					a = tmp[4].Src2;
					while(data[a] != 0) printf("%c", data[a++]);
					break;
				case 5 :
					fscanf(stdin, "%lld", &tmp[4].Result);
					break;
				case 8 :
					fscanf(stdin, "%s", c_tmp);
					a = strlen(c_tmp);
					for(int i = 0; i < a; ++i) {
						data[tmp[4].Src2 + i] = c_tmp[i];
					}
					break;
				case 9 :
					tmp[4].Result = e_pos;
					e_pos += tmp[4].Src2;
					break;
			}
	}
}

bool Write_Back() {
	int now = _order[order[tmp[5].Pos].op];
	if(tmp[5].Rdest == 32) {
		long long e = 1;
		f[32] = tmp[5].Result % (e << 32);
		f[33] = tmp[5].Result >> 32;
		return false;
	}
	if(now <= 22) {
		f[tmp[5].Rdest] = tmp[5].Result;
		return false;
	}
	if(now <= 37) {
		return tmp[5].Result;
	}
	if(now <= 39) {
		f[tmp[5].Rdest] = tmp[5].Result;
		return true;
	}
	if(now <= 43) {
		f[tmp[5].Rdest] = tmp[5].Result;
		return false;
	}
	if(now <= 46) {
		return false;
	}
	if(now <= 49) {
		f[tmp[5].Rdest] = tmp[5].Result;
		return false;
	}
	if(now == 51) {
		if(tmp[5].Src1 == 5 || tmp[5].Src1 == 9) {
			f[tmp[5].Rdest] = tmp[5].Result;
		}
		if(tmp[5].Src1 == 10 || tmp[5].Src1 == 17) return true;
		return false;
	}
	return false;
}

int work(){
	int ans = 0;
	bool Flag = true;
	PC_register = r_Label["main"];
	tmp[1].r_flag = true;
	while(Flag) {
        // cout<<PC_register<<endl;
        // for (int i = 1; i <= 5; i++) {
		// 	cout <<i<<":"<< tmp[i].Pos << " " << tmp[i].r_flag << " " << order[tmp[i].Pos].op<<" "
		// 		<<tmp[i].Rdest<<" "<<tmp[i].Src1<<" "<<tmp[i].Src2<<" "<<tmp[i].Rdest<<" "<<tmp[i].Result<<" "<<tmp[i].Rsrc1<<" "<<tmp[i].Rsrc2<<" "<<tmp[i].address<< endl;
		// }
		if(tmp[5].r_flag) {
			if(Write_Back()) {
				PC_register = tmp[5].address;
				if(tmp[4].Rdest >= 0) judge[tmp[4].Rdest]--;
				if(tmp[4].Rdest == 32) judge[33]--;
				tmp[4].clean();
				if(tmp[3].Rdest >= 0) judge[tmp[3].Rdest]--;
				if(tmp[3].Rdest == 32) judge[33]--;
				tmp[3].clean();
				tmp[2].clean();
				tmp[1].clean();
				if(_order[order[tmp[5].Pos].op] == 51) {
					if(tmp[5].Src1 == 10 || tmp[5].Src1 == 17) {
						ans = tmp[5].Src2;
						Flag = false;
					}
				}
				else tmp[1].r_flag = true;
			}
			if(tmp[5].Rdest >= 0) judge[tmp[5].Rdest]--;
			if(tmp[5].Rdest == 32) judge[33]--;
			tmp[5].clean();
		}
		if(tmp[4].r_flag) {
			Memory_Access();
			tmp[5] = tmp[4];
			tmp[4].clean();
		}
		if(tmp[3].r_flag) {
			Execution();
			tmp[4] = tmp[3];
			tmp[3].clean();
		}
		if(tmp[2].r_flag) {
			if(!Data_Preparation()) continue;
			if(tmp[2].Rdest >= 0) judge[tmp[2].Rdest]++;
			if(tmp[2].Rdest == 32) judge[33]++;
			tmp[3] = tmp[2];
			tmp[2].clean();
		}
		if(tmp[1].r_flag) {
			tmp[1].Pos = PC_register;
			tmp[2] = tmp[1];
			PC_register++;
			if(PC_register >= r_pos) tmp[1].r_flag = false;
		}
	}
	return ans;
}

int main(int argc, char *argv[]) {
	pF = fopen(argv[1], "r");
	init();
	GetIn();
	int ans;
	ans = work();
	return ans;
}
