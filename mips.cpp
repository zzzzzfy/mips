#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<algorithm>
#include<map>
#include<sstream>
#include<vector>
using namespace std;

#define _pc _r[34]
//istringstream is(str)
//is>>kk;

typedef long long LL;
vector<string> vc;
bool sys_end;
int sys_return;
map<string, int> _register, data_label, text_label;
enum _order_value {
	_add, _addu, _addiu, _sub, _subu, _mul, _mulu, _div, _divu, _xor, _xoru, _neg, _negu, _rem, _remu, _li, _seq, _sge, _sgt, _sle, _slt, _sne, _b, _beq, _bne, _bge, _ble, _bgt, _blt, _beqz, _bnez, _blez, _bgez, _bgtz, _bltz, _j, _jr, _jal, _jalr, _la, _lb, _lh, _lw, _sb, _sh, _sw, _move, _mfhi, _mflo, _nop, _syscall
};
map<string, _order_value> order_map;
struct pipeline {
	int pos, address, rdest, rsrc1, src1, rsrc2, src2;
	long long result;
	bool flag;
}pipe[8];

struct order {
	string name, para[5];
	int num;
	int val;
	bool flag;
}order[10000];
int _r[40];
int used[40];
const int mod = 1 << 8;
int head_top, order_num;
unsigned char ram[4 * 1024 * 1024 + 5];
bool isdigital(char c) {
	return ((c <= '9') && (c >= '0')) || (c == '-');
}
void memory_layout() {
	string op;
	bool op_data = false;
	for (int i = 0; i < vc.size(); i++) {
		if (vc[i].length() == 0 || vc[i].length() == 1) continue;
		istringstream _line(vc[i]);
		//cout << vc[i] <<" "<<vc[i].length()<< endl;
		_line >> op;
		int len = op.length();
		if (op == ".data") {
			op_data = true;
			continue;
		}
		if (op == ".text") {
			op_data = false;
			continue;
		}
		if (op[len - 1] == ':') {
			if (op_data) data_label[op.substr(0, len - 1)] = head_top;
			else text_label[op.substr(0, len - 1)] = order_num;
		}
		else if (op[0] == '.') {
			if (op == ".align") {//align
				int _size;
				_line >> _size;
				_size = (1 << _size);
				head_top += ((_size) - head_top % (_size));
				continue;
			}
			if (op[2] == 's') {//ascii & asciiz
				string tmp = vc[i].substr(vc[i].find('\"'), vc[i].length());
				//cout << tmp <<" "<<tmp.length()<<" "<<head_top<<" ";
				for (int i = 1; i <= tmp.length() - 2; i++) {
					if (tmp[i] == '\\') {
						if (tmp[i + 1] == '0') ram[head_top] = '\\';
						if (tmp[i + 1] == 'n') ram[head_top] = '\n';
						if (tmp[i + 1] == 't') ram[head_top] = '\t';
						if (tmp[i + 1] == 'r') ram[head_top] = '\r';
						if (tmp[i + 1] == '\'') ram[head_top] = '\'';
						if (tmp[i + 1] == '\"') ram[head_top] = '\"';
						i++;
						head_top++;
						continue;
					}
					ram[head_top] = tmp[i];
					head_top++;
					
				}
				if (op[len - 1] == 'z') head_top++;
				//cout << head_top << endl;
				continue;
			}
			if (op == ".byte") {
				while (_line) {
					int tmp_byte;
					_line >> op;
					if (op[op.length() - 1] == ',') tmp_byte = atoi(op.substr(0, op.length() - 1).data());
					else tmp_byte = atoi(op.data());
					ram[head_top++] = tmp_byte;
					if (op[op.length() - 1] != ',') break;
				}
				continue;
			}
			if (op == ".half") {
				while (_line) {
					int tmp_byte;
					_line >> op;
					if (op[op.length() - 1] == ',') tmp_byte = atoi(op.substr(0, op.length() - 1).data());
					else tmp_byte = atoi(op.data());
					ram[head_top++] = (tmp_byte >> 8);
					ram[head_top++] = (tmp_byte) % mod;
					if (op[op.length() - 1] != ',') break;
				}
				continue;
			}
			if (op == ".word") {
				while (_line) {
					int tmp_byte;
					_line >> op;
					//if (op == "") break;
					//cout << op << ",";
					if (op[op.length() - 1] == ',') tmp_byte = atoi(op.substr(0, op.length() - 1).data());
					else tmp_byte = atoi(op.data());
					ram[head_top++] = (tmp_byte >> 24) % mod;
					ram[head_top++] = (tmp_byte >> 16) % mod;
					ram[head_top++] = (tmp_byte >> 8) % mod;
					ram[head_top++] = (tmp_byte) % mod;
					if (op[op.length() - 1] != ',') break;
				}
				//cout << endl;
				continue;
			}
			if (op == ".space") {
				int n;
				_line >> n;
				head_top += n;
				continue;
			}
		}
		else {
			order[order_num].name = op;
			while (_line) {
				_line >> op;
				if (op == "") break;
				len = op.length();
				//cout << op << " ";
				//cout << op << " " ;
				if (op[len - 1] == ')') {
					//cout << op << endl;
					int cur = 1;
					while (isdigital(op[cur])) cur++;
					//cout << op << " " <<cur<<" "<<order[order_num].num<<" "<< len << endl;
					order[order_num].val = atoi(op.substr(0, cur).data());
					order[order_num].para[order[order_num].num++] = op.substr(cur + 1, len - cur - 2);
					//cout << order[order_num].para[1] << endl;
					order[order_num].flag = true;
					break;
				}
				else if (op[len - 1] == ',') {
					if (isdigital(op[0])) {
						order[order_num].flag = true;
						order[order_num].val = atoi(op.substr(0,len-1).data());
					}
					else order[order_num].para[order[order_num].num++] = op.substr(0, len - 1);
				}
				else {
					if (isdigital(op[0])) {
						order[order_num].flag = true;
						order[order_num].val = atoi(op.data());
					}
					else {
						order[order_num].para[order[order_num].num++] = op;
					}
					break;
				}
			}
			//cout << endl;
			order_num++;
		}
	}

}
void write_back() {
	if (!pipe[4].flag) return;
	bool token = false;
	int pos = pipe[4].pos;
	if (pipe[4].rdest == 32) {
		_r[33] = pipe[4].result >> 32;
		_r[32] = pipe[4].result % ((LL(1)) << 32);
	}
	else switch (order_map[order[pos].name]) {
	case _add:case _addu:case _addiu:case _sub:case _subu:case _mul:case _mulu:case _div:case _divu:
	case _xor:case _xoru:case _neg:case _negu:case _rem:case _remu:case _li:case _seq:case _sge:
	case _sgt:case _sle:case _slt:case _sne:
		_r[pipe[4].rdest] = pipe[4].result;
		break;
	case _b:case _beq:case _bne:case _bge:case _ble:case _bgt:case _blt:case _beqz:case _bnez:
	case _blez:case _bgez:case _bgtz:case _bltz:case _j:case _jr:
		token = pipe[4].result;
		break;
	case _jal:case _jalr:
		_r[pipe[4].rdest] = pipe[4].result;
		token = true;
		break;
	case _la:case _lb:case _lh:case _lw:case _move:case _mfhi:case _mflo:
		_r[pipe[4].rdest] = pipe[4].result;
		break;
	case _syscall:
		if (pipe[4].src1 == 10 || pipe[4].src1 == 17) 
			token = true;
		if (pipe[4].src1 == 5 || pipe[4].src1 == 9)
			_r[pipe[4].rdest] = pipe[4].result;
		break;
	}
	if (token) {
		_pc = pipe[4].address;
		if (pipe[3].flag&&pipe[3].rdest > 0) used[pipe[3].rdest]--;
		if (pipe[3].flag&&pipe[3].rdest == 32) used[33]--;
		if (pipe[2].flag&&pipe[2].rdest > 0) used[pipe[2].rdest]--;
		if (pipe[2].flag&&pipe[2].rdest == 32) used[33]--;
		pipe[3].flag = pipe[2].flag = pipe[1].flag = pipe[0].flag = false;
		if (order_map[order[pipe[4].pos].name] == _syscall) {
			if (pipe[4].src1 == 10 || pipe[4].src1 == 17) {
				sys_return = pipe[4].src2;
				sys_end = true;
			}
		}
		else pipe[0].flag = true;
	}
	if (pipe[4].rdest > 0) used[pipe[4].rdest]--;
	if (pipe[4].rdest == 32) used[33]--;
	pipe[4].flag = false;
}
void access() {
	if (!pipe[3].flag) return;
	int pos = pipe[3].pos;
	string t;
	switch (order_map[order[pos].name]) {
	case _lb:
		pipe[3].result = ram[pipe[3].address];
		break;
	case _lh:
		pipe[3].result = (ram[pipe[3].address] << 8) | (ram[pipe[3].address + 1]);
		break;
	case _lw:
		pipe[3].result = ((ram[pipe[3].address]) << 24) | ((ram[pipe[3].address + 1]) << 16)
			| ((ram[pipe[3].address + 2]) << 8) | ((ram[pipe[3].address + 3]));
		break;
	case _sb:
		ram[pipe[3].address] = pipe[3].src1 % mod;
		break;
	case _sh:
		ram[pipe[3].address] = (pipe[3].src1 >> 8) % mod;
		ram[pipe[3].address + 1] = pipe[3].src1 % mod;
		break;
	case _sw:
		ram[pipe[3].address] = (pipe[3].src1 >> 24);
		ram[pipe[3].address + 1] = (pipe[3].src1 >> 16) % mod;
		ram[pipe[3].address + 2] = (pipe[3].src1 >> 8) % mod;
		ram[pipe[3].address + 3] = (pipe[3].src1) % mod;
		break;
	case _syscall:
		switch (pipe[3].src1){
			//cout <<"syscall"<<":"<< pipe[3].src1 << endl;
		case 1:
			//cout <<pipe[3].pos<<" "<<order[pipe[3].pos].name<< "aa:" << endl;
			cout << pipe[3].src2 << "\n";
			break;
		case 4:
			//cout <<"a:"<< pipe[3].src2 <<" "<<pipe[3].pos<< endl;
			//for (int i = 0; i < head_top; i++)
			//	cout << int(ram[i]) << " ";
			//cout << endl;
			for (int i = pipe[3].src2; ram[i]; i++) {
				cout << char(ram[i]);
			}
			break;
		case 5:
			cin >> pipe[3].result;
			break;
		case 8:
			{	
				cin >> t; 
				int len = t.length();
				for (int i = 0; i < len; i++)
					ram[pipe[3].src2 + i] = t[i];
			}
			break;
		case 9:
			pipe[3].result = head_top;
			head_top += pipe[3].src2;
			break;
		}
	}
	pipe[4] = pipe[3];
	pipe[3].flag = false;
}
void execution() {
	if (!pipe[2].flag) return;
	int pos = pipe[2].pos;
	switch (order_map[order[pos].name]){
	case _add:
		pipe[2].result = pipe[2].src1 + pipe[2].src2;
		break;
	case _addu:case _addiu:
		pipe[2].result = (unsigned int)pipe[2].src1 + (unsigned int)pipe[2].src2;
		break;
	case _sub:
		pipe[2].result = pipe[2].src1 - pipe[2].src2;
		break;
	case _subu:
		pipe[2].result = (unsigned int)pipe[2].src1 - (unsigned int)pipe[2].src2;
		break;
	case _mul:
		pipe[2].result = ((LL)pipe[2].src1)*pipe[2].src2;
		break;
	case _mulu:
		pipe[2].result = ((unsigned long long)pipe[2].src1) * ((unsigned long long)pipe[2].src2);
		break;
	case _div: {
		LL md = pipe[2].src1 % pipe[2].src2;
		pipe[2].result = (md << 32) | (pipe[2].src1 / pipe[2].src2);
		break;
	}
	case _divu: {
		LL md = ((unsigned long long)pipe[2].src1) % ((unsigned long long)pipe[2].src2);
		pipe[2].result = (md << 32) | ((unsigned long long)pipe[2].src1) / ((unsigned long long)pipe[2].src2);
		break;
	}
	case _xor:
		pipe[2].result = pipe[2].src1 ^ pipe[2].src2;
		break;
	case _xoru:
		pipe[2].result = ((unsigned int)pipe[2].src1) ^ pipe[2].src2;
		break;
	case _neg:
		pipe[2].result = -pipe[2].src1;
		break;
	case _negu:
		pipe[2].result = ~((unsigned int)pipe[2].src1);
		break;
	case _rem:
		pipe[2].result = pipe[2].src1 % pipe[2].src2;
		break;
	case _remu:
		pipe[2].result = ((unsigned int)pipe[2].src1) % ((unsigned int)pipe[2].src2);
		break;
	case _li:
		pipe[2].result = pipe[2].src1;
		break;
	case _seq:case _beq:case _beqz:
		pipe[2].result = (pipe[2].src1 == pipe[2].src2);
		break;
	case _sge:case _bge:case _bgez:
		//cout << "bge:" << pipe[2].src1 << " " << pipe[2].src2 << endl;
		pipe[2].result = (pipe[2].src1 >= pipe[2].src2);
		break;
	case _sgt:case _bgt:case _bgtz:
		pipe[2].result = (pipe[2].src1 > pipe[2].src2);
		break;
	case _sle:case _ble:case _blez:
		pipe[2].result = (pipe[2].src1 <= pipe[2].src2);
		break;
	case _slt:case _blt:case _bltz:
		pipe[2].result = (pipe[2].src1 < pipe[2].src2);
		break;
	case _sne:case _bne:case _bnez:
		pipe[2].result = (pipe[2].src1 != pipe[2].src2);
		break;
	case _b:case _j: case _jr:
		pipe[2].result = 1;
		break;
	case _jal:case _jalr:
		pipe[2].result = pos + 1;
		break;
	case _la:
		pipe[2].result = pipe[2].address;
		break;
	case _move:case _mfhi:case _mflo:
		pipe[2].result = pipe[2].src1;
		break;
	}
	pipe[3] = pipe[2];
	pipe[2].flag = false;
}
bool preparation() {
	if (!pipe[1].flag) return true;
	int pos = pipe[1].pos;//order pos
	switch (order_map[order[pos].name]) {
	case _add:case _sub:case _xor:case _rem:case _seq:case _sge:case _sgt:case _sle:case _slt:
	case _sne:case _addu:case _addiu:case _subu:case _xoru:case _remu:
		pipe[1].rdest = _register[order[pos].para[0]];
		pipe[1].rsrc1 = _register[order[pos].para[1]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		if (order[pos].flag) pipe[1].src2 = order[pos].val;
		else {
			pipe[1].rsrc2 = _register[order[pos].para[2]];
			if (used[pipe[1].rsrc2]) return false;
			pipe[1].src2 = _r[pipe[1].rsrc2];
		}
		break;
	case _mul:case _div:case _mulu:case _divu:
		if ((order[pos].flag && order[pos].num == 2) || order[pos].num == 3) {
			pipe[1].rdest = _register[order[pos].para[0]];
			pipe[1].rsrc1 = _register[order[pos].para[1]];
			if (used[pipe[1].rsrc1]) return false;
			pipe[1].src1 = _r[pipe[1].rsrc1];
			if (order[pos].flag) pipe[1].src2 = order[pos].val;
			else {
				pipe[1].rsrc2 = _register[order[pos].para[2]];
				if (used[pipe[1].rsrc2]) return false;
				pipe[1].src2 = _r[pipe[1].rsrc2];
			}
		}
		else {
			pipe[1].rdest = _register["$lo"];
			pipe[1].rsrc1 = _register[order[pos].para[0]];
			if (used[pipe[1].rsrc1]) return false;
			pipe[1].src1 = _r[pipe[1].rsrc1];
			if (order[pos].flag) pipe[1].src2 = order[pos].val;
			else {
				pipe[1].rsrc2 = _register[order[pos].para[1]];
				if (used[pipe[1].rsrc2]) return false;
				pipe[1].src2 = _r[pipe[1].rsrc2];
			}
		}
		break;
	case _neg:case _negu:
		pipe[1].rdest = _register[order[pos].para[0]];
		pipe[1].rsrc1 = _register[order[pos].para[1]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		break;
	case _li:
		//cout << order[pos].para[0] << endl;
		pipe[1].rdest = _register[order[pos].para[0]];
		pipe[1].src1 = order[pos].val;
		break;
	case _b:
		pipe[1].address = text_label[order[pos].para[0]];
		break;
	case _beq:case _bne:case _bge:case _ble:case _bgt:case _blt:
		pipe[1].rsrc1 = _register[order[pos].para[0]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		if (order[pos].flag) pipe[1].src2 = order[pos].val;
		else {
			pipe[1].rsrc2 = _register[order[pos].para[1]];
			if (used[pipe[1].rsrc2]) return false;
			pipe[1].src2 = _r[pipe[1].rsrc2];
		}
		pipe[1].address = text_label[order[pos].para[order[pos].num - 1]];
		break;
	case _beqz:case _bnez:case _blez:case _bgez:case _bgtz:case _bltz:
		pipe[1].rsrc1 = _register[order[pos].para[0]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		pipe[1].address = text_label[order[pos].para[order[pos].num - 1]];
		pipe[1].src2 = 0;
		break;
	case _j:
		pipe[1].address = text_label[order[pos].para[0]];
		break;
	case _jr:
		pipe[1].rsrc1 = _register[order[pos].para[0]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].address = _r[pipe[1].rsrc1];
		break;
	case _jal:
		pipe[1].rdest = _register["$31"];
		pipe[1].address = text_label[order[pos].para[0]];
		break;
	case _jalr:
		pipe[1].rdest = _register["$31"];
		pipe[1].rsrc1 = _register[order[pos].para[0]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].address = _r[pipe[1].rsrc1];
		break;
	case _la:case _lb:case _lh:case _lw:
		pipe[1].rdest = _register[order[pos].para[0]];
		if (order[pos].flag) {
			pipe[1].rsrc1 = _register[order[pos].para[1]];
			if (used[pipe[1].rsrc1]) return false;
			pipe[1].address = _r[pipe[1].rsrc1] + order[pos].val;
		}
		else pipe[1].address = data_label[order[pos].para[1]];
		break;
	case _sb:case _sh:case _sw:
		pipe[1].rsrc1 = _register[order[pos].para[0]];
		//cout << order[pos].para[0] << endl;
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		if (order[pos].flag) {
			pipe[1].rsrc2 = _register[order[pos].para[1]];
			if (used[pipe[1].rsrc2]) return false;
			pipe[1].address = _r[pipe[1].rsrc2] + order[pos].val;
		}
		else pipe[1].address = data_label[order[pos].para[1]];
		break;
	case _move:
		pipe[1].rdest = _register[order[pos].para[0]];
		pipe[1].rsrc1 = _register[order[pos].para[1]];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		break;
	case _mfhi:
		pipe[1].rdest = _register[order[pos].para[0]];
		pipe[1].rsrc1 = _register["$hi"];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		break;
	case _mflo:
		pipe[1].rdest = _register[order[pos].para[0]];
		pipe[1].rsrc1 = _register["$lo"];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		break;
	case _syscall:
		pipe[1].rsrc1 = _register["$v0"];
		if (used[pipe[1].rsrc1]) return false;
		pipe[1].src1 = _r[pipe[1].rsrc1];
		if (pipe[1].src1 == 1 || pipe[1].src1 == 4 || pipe[1].src1 == 8 || pipe[1].src1 == 9 || pipe[1].src1 == 17) {
			if (pipe[1].src1 == 9) pipe[1].rdest = _register["$v0"];
			pipe[1].rsrc2 = _register["$a0"];
			if (used[pipe[1].rsrc2]) return false;
			pipe[1].src2 = _r[pipe[1].rsrc2];
			break;
		}
		if (pipe[1].src1 == 5) {
			pipe[1].rdest = _register["$v0"];
		}
		break;
	}
	//cout <<" :"<< pipe[1].rdest <<" "<<used[pipe[1].rdest]<< endl;
	if (pipe[1].rdest > 0) used[pipe[1].rdest]++;
	//cout << " :" << pipe[1].rdest << " " << used[2] << endl;
	if (pipe[1].rdest == 32) used[33]++;
	pipe[2] = pipe[1];
	pipe[1].flag = 0;
	return true;
}
void fetch() {
	if (!pipe[0].flag) return;
	pipe[0].pos = _pc;
	pipe[1] = pipe[0];
	_pc++; 
	if (_pc >= order_num) pipe[0].flag = false;
}
int kz;
void work() {//34 -> pc register
	_pc = text_label["main"];
	pipe[0].flag = true;
	while (!sys_end) {
		kz++;
		////cout << "!!" << endl;
		////system("pause");
		/*cout << _pc << endl;
		for (int i = 0; i <= 4; i++) {
			cout <<i<<":"<< pipe[i].pos << " " << pipe[i].flag << " " << order[pipe[i].pos].name<<" "
				<<pipe[i].rdest<<" "<<pipe[i].src1<<" "<<pipe[i].src2<<" "<<pipe[i].rdest<<" "<<pipe[i].result<<" "<<pipe[i].rsrc1<<" "<<pipe[i].rsrc2<<" "<<pipe[i].address<< endl;
		}*/

		////cout << _pc << endl;
		////for (int i = 0; i < head_top; i++) {
		////	cout << int(ram[i]) << " ";
		////}
		////cout << endl;
		//for (int i = 0; i <= 34; i++) {
		//	cout << i << ":" << _r[i] << " ";
		//}
		//cout << endl;
		//cout << "====================" << endl;
		//if (_pc >= 270) system("pause");
		write_back();
		//cout << "!" << endl;
		access();
		//cout << "!" << endl;
		execution();
		//cout << "!" << endl;
		if (!preparation()) continue;
		//cout << "!" << endl;
		fetch();
		//cout << "!" << endl;
		//cout << "zzzzzzzzzz" << endl;
		//for (int i = 0; i <= 4; i++) {
		//	cout << i << ":" << pipe[i].pos << " " << pipe[i].flag << " " << order[pipe[i].pos].name << endl;
		//}
		//cout << _r[31] << endl;
		//cout << "====================" << endl;
	}
}
int main(int argc, char **argv) {
	//freopen("22.in", "r", stdin);
	//freopen("a.out", "w", stdout);
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
	_register["$30"] = _register["$fp"] = 30;
	_register["$31"] = _register["$ra"] = 31;
	_register["$lo"] = 32;
	_register["$hi"] = 33;
	_r[29] = 4000000;
	order_map["add"] = _add;
	order_map["addu"] = _addu;
	order_map["addiu"] = _addiu;
	order_map["sub"] = _sub;
	order_map["subu"] = _subu;
	order_map["mul"] = _mul;
	order_map["mulu"] = _mulu;
	order_map["div"] = _div;
	order_map["divu"] = _divu;
	order_map["xor"] = _xor;
	order_map["xoru"] = _xoru;
	order_map["neg"] = _neg;
	order_map["negu"] = _negu;
	order_map["rem"] = _rem;
	order_map["remu"] = _remu;
	order_map["li"] = _li;
	order_map["seq"] = _seq;
	order_map["sge"] = _sge;
	order_map["sgt"] = _sgt;
	order_map["sle"] = _sle;
	order_map["slt"] = _slt;
	order_map["sne"] = _sne;
	order_map["b"] = _b;
	order_map["beq"] = _beq;
	order_map["bne"] = _bne;
	order_map["bge"] = _bge;
	order_map["ble"] = _ble;
	order_map["bgt"] = _bgt;
	order_map["blt"] = _blt;
	order_map["beqz"] = _beqz;
	order_map["bnez"] = _bnez;
	order_map["blez"] = _blez;
	order_map["bgez"] = _bgez;
	order_map["bgtz"] = _bgtz;
	order_map["bltz"] = _bltz;
	order_map["j"] = _j;
	order_map["jr"] = _jr;
	order_map["jal"] = _jal;
	order_map["jalr"] = _jalr;
	order_map["la"] = _la;
	order_map["lb"] = _lb;
	order_map["lh"] = _lh;
	order_map["lw"] = _lw;
	order_map["sb"] = _sb;
	order_map["sh"] = _sh;
	order_map["sw"] = _sw;
	order_map["move"] = _move;
	order_map["mfhi"] = _mfhi;
	order_map["mflo"] = _mflo;
	order_map["nop"] = _nop;
	order_map["syscall"] = _syscall;
	ifstream fin(argv[1]);
	string s;
	while (getline(fin, s)) {
		vc.push_back(s);
	}
	memory_layout();
	//cout << order_num << endl;
	//for (int i = 0; i < order_num; i++) {
	//	cout << i << ":";
	//	cout <<order[i].name<<" "<< order[i].flag << " " << order[i].num<<" "<<order[i].val << " ";
	//	for (int j = 0; j < order[i].num; j++) {
	//		cout << order[i].para[j] << " ";
	//	}
	//	cout << endl;
	//	//cout << order[i].para[0] <<" "<<order[i].val<< " " << endl;
	//}
	//cout << endl;
	work();
	//cout << kz << endl;
	/*freopen("a.out","w",stdout);
	cout << head_top << endl;
	for (int i = 0; i < head_top; i++) {
		cout << int(ram[i]) << " ";
	}*/
	return sys_return;
}
