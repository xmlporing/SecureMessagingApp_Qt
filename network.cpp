#include "network.h"

QString Custom::decrypt(CryptoPP::SecByteBlock secretKey, QByteArray IV, QByteArray cipherText){
    //plaintext, not encoded in base64
    std::string plaintext;

    try{
        //init
        CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption decryptor;
        decryptor.SetKeyWithIV(secretKey, secretKey.size(), (byte*)IV.data(), IV.size());

        //decrypt
        CryptoPP::StringSource( (byte*)cipherText.data(), cipherText.size(), true,
               new CryptoPP::StreamTransformationFilter( decryptor,
                  new CryptoPP::StringSink( plaintext )
               ) // StreamTransformationFilter
        ); // StringSource
    }catch( const CryptoPP::Exception& e){
        qDebug() << e.what();
    }

    //plaintext
    QString decryptedtext = QString::fromStdString(plaintext);

    //return
    return decryptedtext;
}

/*
QByteArray Custom::decrypt(QString secretKey, QString IV, QString cipherText){
    //Convert base64 encoded inputs
    CryptoPP::SecByteBlock key(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    //secret key
    QByteArray byteSecretKey = QByteArray::fromBase64(secretKey.toLocal8Bit());
    for (int i=0; i< byteSecretKey.size(); i++){
        key[i] = byteSecretKey[i];
    }
    //iv
    QByteArray iv = QByteArray::fromBase64(IV.toLocal8Bit());
    //ciphertext
    QByteArray cipher = QByteArray::fromBase64(cipherText.toLocal8Bit());
    //plaintext
    std::string plaintext;

    try{
        //init
        CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption decryptor;
        decryptor.SetKeyWithIV(key, key.size(), (byte*)iv.data(), iv.size());

        //decrypt
        CryptoPP::StringSource( (byte*)cipher.data(), cipher.size(), true,
           new CryptoPP::StreamTransformationFilter( decryptor,
              new CryptoPP::StringSink( plaintext )
           ) // StreamTransformationFilter
        ); // StringSource
    }catch( const CryptoPP::Exception& e){
        qDebug() << e.what();
    }

    //plaintext
    QString decryptedtext = QString::fromStdString(plaintext);
    qDebug() << decryptedtext;

    //return
    return decryptedtext.toLocal8Bit();
}
*/

QString Custom::decrypt(QString secretKey, QString IV, QString cipherText){
    //Convert base64 encoded inputs
    CryptoPP::SecByteBlock key(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
    //secret key
    QByteArray byteSecretKey = QByteArray::fromBase64(secretKey.toLocal8Bit());
    for (int i=0; i< byteSecretKey.size(); i++){
        key[i] = byteSecretKey[i];
    }
    //iv
    QByteArray iv = QByteArray::fromBase64(IV.toLocal8Bit());
    //ciphertext
    QByteArray cipher = QByteArray::fromBase64(cipherText.toLocal8Bit());
    //plaintext
    std::string plaintext;

    try{
        //init
        CryptoPP::CBC_Mode< CryptoPP::AES >::Decryption decryptor;
        decryptor.SetKeyWithIV(key, key.size(), (byte*)iv.data(), iv.size());

        //decrypt
        CryptoPP::StringSource( (byte*)cipher.data(), cipher.size(), true,
           new CryptoPP::StreamTransformationFilter( decryptor,
              new CryptoPP::StringSink( plaintext )
           ) // StreamTransformationFilter
        ); // StringSource
    }catch( const CryptoPP::Exception& e){
        qDebug() << e.what();
    }

    //plaintext
    QString decryptedtext = QString::fromStdString(plaintext);

    //return
    return decryptedtext;
}

QByteArray Custom::encrypt(CryptoPP::SecByteBlock secretKey, QByteArray& IV, QString plaintext){
    //plaintext, not encoded in base64
    QByteArray plain = plaintext.toLocal8Bit();
    //ciphertext
    std::string ciphertext;
    //generate IV
    byte iv[ CryptoPP::AES::BLOCKSIZE ];
    CryptoPP::AutoSeededRandomPool prng;
    prng.GenerateBlock( iv, sizeof(iv) );
    // convert to QByteArray that is passed by reference
    IV = QByteArray((char *)iv, sizeof(iv));
    try{
        //init
        CryptoPP::CBC_Mode< CryptoPP::AES >::Encryption encryptor;
        encryptor.SetKeyWithIV(secretKey, secretKey.size(), (byte*)IV.data(), IV.size());

        //decrypt
        CryptoPP::StringSource( (byte*)plain.data(), plain.size(), true,
           new CryptoPP::StreamTransformationFilter( encryptor,
              new CryptoPP::StringSink( ciphertext )
           ) // StreamTransformationFilter
        ); // StringSource
    }catch( const CryptoPP::Exception& e){
        qDebug() << e.what();
    }

    //ciphertext
    QByteArray encryptedText(ciphertext.c_str(), ciphertext.length());

    //return
    return encryptedText;
}

bool Custom::setKey(CryptoPP::SecByteBlock& secretKey, QString stringKey){
    QByteArray byteSecretKey = QByteArray::fromBase64(stringKey.toLocal8Bit());
    //check for correct key size, 128 bits/16bytes
    if (byteSecretKey.size() != CryptoPP::AES::DEFAULT_KEYLENGTH)
        return false;
    //insert key
    for (int i=0; i< byteSecretKey.size(); i++){
        secretKey[i] = byteSecretKey[i];
    }
    return true;
}

void Custom::nonceIncrement(int &nonce){
    //check if overflow
    if (nonce == std::numeric_limits<int>::max() - 1){
        nonce = std::numeric_limits<int>::min();
    }else{
        //increase nonce
        nonce++;
    }
}
