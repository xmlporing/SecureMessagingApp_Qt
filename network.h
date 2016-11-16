#ifndef NETWORK_H
#define NETWORK_H

#define PORT 1234
#define TIMEOUT 5000

#include <QObject>
#include <QDebug>
#include <QDataStream>

//Encryption (Crypto++)
#include "aes.h"
#include "ccm.h"
#include "modes.h"
#include "filters.h"
#include "osrng.h" //generate random bits

//std string
#include <string>

/*
 * All value in bits
 * Init             -> HEADER
 * TokenVerify      -> HEADER | token(16)
 * ServerVerify     -> HEADER | IV(16) | E(token(16)|session_key(base64))
 * SendNonce        -> HEADER | IV(16) | E("session_nonce,id,")
 * UserDetails      -> HEADER | IV(16) | E(username)
 * Message          -> HEADER | IV(16) | E("session_nonce,userid,msg")
 * ClientJoin       -> HEADER | IV(16) | E(",userid,username")
 * ClientQuit       -> HEADER | IV(16) | E(userid)
 * HostQuit         -> HEADER | IV(16) | E(session_nonce)
 * Reject           -> HEADER | Error_code(8)
 */

/*
 * Type(2bytes) | Length(4 bytes) | Value (IV(16 bytes) | ciphertext(n * 16 bytes))
 */
enum PROTOCOL {
    Type = 0,
    TypeSize = 2,
    Length = 2,
    LengthSize = 4,
    HeaderSize = 6,
    Value = 6,
    IV = 6,
    IVSize = CryptoPP::AES::BLOCKSIZE,
    CipherText = IV + IVSize,
    CipherTextBlockSize = CryptoPP::AES::BLOCKSIZE,
    ErrorSize = 1,
    TokenSize = 16,
    NonceSize = 4,
};
#define TType uint16_t
#define LType uint32_t
#define EType uint8_t

enum PROTOCOL_TYPE {
    Init,           //Client send to server request to join
    TokenVerify,    //Server send to client token
    ServerVerify,   //Client send to server authentication ticket
    SendNonce,      //Server send nonce
    UserDetails,    //Client send username to server
    Message,        //Client is verified and send msg
    ClientJoin,
    ClientQuit,
    HostQuit,
    Reject,
    ForceSize = 0xFFFF, //Do not use, used to ensure is 2 bytes
};

//Message protocol, (nonce,userid,value), have delimiter char of ","
#define DELIMITER ","
enum MESSAGE{
    Section = 3,
    Nonce = 0,
    UserId = 1,
    MsgValue = 2,
};

enum ERROR{
    InvalidSize,
    InvalidToken,
    InvalidSessionNonce,
    InvalidUser,
    InvalidMsg,
    RoomFull,
    UnrecognisedPacket,
    ForceSize2 = 0xFF, //Do not user, used to ensure is 1 byte
};

#define DATASTREAMVER QDataStream::Qt_5_7

namespace Custom{
    QString decrypt(CryptoPP::SecByteBlock secretKey, QByteArray IV, QByteArray cipherText);
    QString decrypt(QString secretKey, QString IV, QString cipherText);
    QByteArray encrypt(CryptoPP::SecByteBlock secretKey, QByteArray& IV, QString plaintext);
    bool setKey(CryptoPP::SecByteBlock& secretKey, QString stringKey);
    void nonceIncrement(int& nonce);
}


#endif // NETWORK_H
