/*
  RLP.cpp - RLP library for RLP functions
*/
#include "RLP.h"
#include <iostream>
using namespace std;

string RLP::encode(string s)
{
    if (s.size()==0) {
        return hexToBytes("80");
    }
  	else if(s.size()==1 && (unsigned char)s[0]<128)
  		return s;
	else{
		return encodeLength(s.size(), 128)+s;
	}
}
string RLP::encode(TX tx, bool toSign)
{
    string serialized = hexToRlpEncode(tx.nonce)+
                        hexToRlpEncode(tx.gasPrice)+
                        hexToRlpEncode(tx.gasLimit)+
                        hexToRlpEncode(tx.to)+
                        hexToRlpEncode(tx.value)+
                        hexToRlpEncode(tx.data);


    if(!toSign)
          serialized += hexToRlpEncode(tx.v)+
                        hexToRlpEncode(tx.r)+
                        hexToRlpEncode(tx.s);
//    cout << "hexToRlpEncode(tx.v).length() is " << bytesToHex(hexToRlpEncode(tx.v)) << endl;
//    cout << "hexToRlpEncode(tx.r).length() is " << bytesToHex(hexToRlpEncode(tx.r)) << endl;
//    cout << "hexToRlpEncode(tx.s).length() is " << bytesToHex(hexToRlpEncode(tx.s)) << endl;
//    cout << "hexToRlpEncode(tx.nonce).length() is " << hexToRlpEncode(tx.nonce).length() << endl;
//    cout << "hexToRlpEncode(tx.gasPrice).length() is " << hexToRlpEncode(tx.gasPrice).length() << endl;
//    cout << "hexToRlpEncode(tx.gasLimit).length() is " << hexToRlpEncode(tx.gasLimit).length() << endl;
//    cout << "hexToRlpEncode(tx.to).length() is " << hexToRlpEncode(tx.to).length() << endl;
//    cout << "hexToRlpEncode(tx.value).length() is " << hexToRlpEncode(tx.value).length() << endl;
    /*cout << "hexToRlpEncode(tx.data).length() is " << hexToRlpEncode(tx.data).length() << endl;
    cout << "serialized.length() is " << serialized.length() << endl;
    cout << "encodeLength(serialized.length(),192) is " << encodeLength(serialized.length(),192) << endl;
    cout << "bytesToHex " << bytesToHex(hexToBytes(encodeLength(serialized.length(),192))) << endl;*/

    return hexToBytes(encodeLength(serialized.length(),192))+serialized;
//    if (!toSign) {
//      return hexToBytes(encodeLength(serialized.length(),192))+serialized;
//    } else {
//      return hexToBytes(encodeLength(serialized.length()+3,192))+serialized;
//    }

}
string RLP::hexToBytes(string s){
    if (s.size() == 0) {
        return "";
    }
    //TODO check if even length
    char inp [s.length()] = {};
	memcpy(inp,s.c_str(),s.length());
    char dest [sizeof(inp)/2] = {};
	hex2bin(inp,dest);
	return string(dest,sizeof(dest));
}
string RLP::hexToRlpEncode(string s){
    s = removeHexFormatting(s);
	return encode(hexToBytes(s));
}
string RLP::removeHexFormatting(string s){
    if(s[0]=='0'&&s[1]=='x')
        return s.substr(2,s.length()-2);
    return s;
}
string RLP::encodeLength(int len, int offset)
{
	string temp;
  	if(len<56){
      //cout << "len<56" << endl;
  		temp=(char)(len+offset);
      //cout << "bytesToHex inside encodeLength is " << bytesToHex(temp) << endl;
  		return temp;
  	}else{
      //cout << "!!!!!!!!!!!!" << endl;
  		string hexLength = intToHex(len);
		int	lLength =   hexLength.length()/2;
		string fByte = intToHex(offset+55+lLength);
		return fByte+hexLength;
	}
}
string RLP::intToHex(int n){
	stringstream stream;
	stream << std::hex << n;
	string result( stream.str() );
	if(result.size() % 2)
		result = "0"+result;
	return result;
}
string RLP::bytesToHex(string input)
{
    static const char* const lut = "0123456789ABCDEF";
    size_t len = input.length();
    std::string output;
    output.reserve(2 * len);
    for (size_t i = 0; i < len; ++i)
    {
        const unsigned char c = input[i];
        output.push_back(lut[c >> 4]);
        output.push_back(lut[c & 15]);
    }
    return output;
}
int RLP::char2int(char input)
{
  if(input >= '0' && input <= '9') {
    return input - '0';
  }

  if(input >= 'A' && input <= 'F') {
    return input - 'A' + 10;
  }

  if(input >= 'a' && input <= 'f') {
    return input - 'a' + 10;
  }
}
void RLP::hex2bin(const char* src, char* target)
{
    const char *original = target;
  while(*src)
  {
      if (src[1]) {
        *(target++) = char2int(*src)*16 + char2int(src[1]);
      } else {
        *(target++) = char2int(*src);
      }

    src += 2;
  }
}
