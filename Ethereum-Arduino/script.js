var rawTx = {
            nonce: '0x05',
            gasPrice: '0x210000', 
            gasLimit: '0x100000',
            to: '0x0000000000000000000000000000000000000000', 
            value: '0x10000000', 
            data: '0x7f7465737432000000000000000000000000000000000000000000000000000000600057',
            chainId:15
        }

var TX = require('ethereumjs-tx')
var privateKey = new Buffer('440ec6f6dc87dda4d2918a07d115ae4d290e290bd92e70e851eeb23de59d955a', 'hex')
var tx = new TX(rawTx)
tx.sign(privateKey)
Buffer.from(tx.serialize()).toString('hex')