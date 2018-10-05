#include <iostream>
#include "RLP.h"
#include <string>
#include "libs/keccak.h"
#include <memory.h>
#include <stdlib.h>
extern "C" {
    #include "libs/ecdsa.h"
    #include "libs/bignum256.h"
}

#define EIP_155_OFFSET 35

uint8_t* charArrtoUint(char src[]){
    uint8_t dest[sizeof(src)];
    for(int i=0;i<sizeof(src);i++){
        dest[i] = (int)src[i];
    }
    return dest;
}
char* uintToCharArr(uint8_t src[], int len){
    char* dest = (char*) calloc(len,1);
    for(int i=0;i<len;i++){
        dest[i] = src[i];
    }
    return dest;
}
void splitArray(uint8_t src[], uint8_t dest[], uint8_t from, uint8_t to)
{
    int i = 0;
	for(int ctr=from; ctr<to; ctr++)
	{
		dest[i]	=  src[ctr];
		i++;
	}
}

void hexstrToByteArray(const char hexstr[], size_t hexstr_size, uint8_t **pp_arr_out) {
    const char *pos = hexstr;
    size_t arr_size = hexstr_size/2, i;
    uint8_t *p_arr = (uint8_t *) malloc(arr_size);
    memset(p_arr,0,arr_size);

    for(int i = 0; i<arr_size;++i) {
        sscanf(pos, "%2hhx", &p_arr[i]);
        pos+=2;
    }

    *pp_arr_out = p_arr;
}

void reverse(uint8_t *p_arr, size_t len) {
    size_t i;
    for(int i = 0; i<len/2; ++i) {
        uint8_t tmp = p_arr[i];
        p_arr[i] = p_arr[len-1-i];
        p_arr[len-1-i] = tmp;
    }
}

void fixHexValue(std::string &hex) {
    if (hex.size() % 2 != 0) {
        RLP rlp;
        std::string tmp = rlp.removeHexFormatting(hex);
        tmp = "0x0" + tmp;
        hex = tmp;
    }
}

void adjustTxValues(TX &tx) {
    fixHexValue(tx.nonce);
    fixHexValue(tx.gasPrice);
    fixHexValue(tx.gasLimit);
    fixHexValue(tx.to);
    fixHexValue(tx.value);
    fixHexValue(tx.data);
    fixHexValue(tx.v);
}

int main(int argc, char** argv) {
    using namespace std;
	RLP rlp;
	TX tx;
    tx.nonce="0x5";
    tx.gasPrice="0x210000";
    tx.gasLimit="0x100000";
    tx.to="0x0000000000000000000000000000000000000000";
    tx.value="0x10000000";
    tx.data="0x7f7465737432000000000000000000000000000000000000000000000000000000600057";
    tx.chainId = 15;
    tx.v=rlp.intToHex(tx.chainId);//as per EIP 155
    tx.r="";
    tx.s="";
    adjustTxValues(tx);

    string privkey = "440ec6f6dc87dda4d2918a07d115ae4d290e290bd92e70e851eeb23de59d955a";
    uint8_t *privkeyBytes;
    hexstrToByteArray(privkey.c_str(), 64, &privkeyBytes);
    char inp [privkey.length()] = {};
	memcpy(inp,privkey.c_str(),privkey.length());
    char dest [sizeof(inp)/2] = {};
	rlp.hex2bin(inp,dest);

    string txRLP = rlp.encode(tx,false);
    Keccak k;
    std::string myHash  = k(txRLP);
    uint8_t *hashBytes;
    hexstrToByteArray(myHash.c_str(), 64, &hashBytes);

    uint8_t r[32];
    uint8_t s[64];
    uint8_t recId;

    ecdsaSign((BigNum256)r, (BigNum256)s, (BigNum256)hashBytes, (BigNum256)privkeyBytes, &recId);
    reverse(r, 32);
    reverse(s, 32);
    char* rCharArr = uintToCharArr(r,32);
    char* sCharArr = uintToCharArr(s,32);
    int vInt = EIP_155_OFFSET + (int) recId + 2 * tx.chainId;
    char *vChar = (char *) calloc(50,1);
    snprintf(vChar,50,"%x",vInt);

    tx.v = string(vChar);
    tx.r = "0x"+rlp.bytesToHex(string(rCharArr,32));
    tx.s = "0x"+rlp.bytesToHex(string(sCharArr,32));
    cout << "r is " << tx.r << endl;
    cout << "s is " << tx.s << endl;
    free(rCharArr);
    free(sCharArr);
    cout << "transaction signed is " << rlp.bytesToHex(rlp.encode(tx,false)) << endl;
}




