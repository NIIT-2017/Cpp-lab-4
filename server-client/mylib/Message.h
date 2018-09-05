#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include "MessageType.h"
#include <zlib.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>


class Message;

struct ConfigMessage
{
    ConfigMessage(bool isCompression = false, bool isEncryption = false,
                  RSA *rsa_private = nullptr, RSA *rsa_public = nullptr);
    bool isCompression;
    bool isEncryption;
    RSA * rsa_private;
    RSA * rsa_public;
};


class Message
{
private:
    MessageType type;
    QString *data;
    bool isInfo;
    ConfigMessage config;

    bool compressMes(QByteArray **);
    bool uncompressMes(QByteArray **, int);
    bool encryptionMes(QByteArray **);
    bool unencryptionMes(QByteArray **);

public:
    Message(ConfigMessage = ConfigMessage(), bool info = false);
    Message(MessageType, ConfigMessage = ConfigMessage(), bool info = false);
    Message(MessageType, QString *, ConfigMessage = ConfigMessage(), bool info = false);
    ~Message();
    MessageType getType() const;
    QString * getData() const;
    QByteArray * writeObject();
    void readObject(const QByteArray &);
    void setInfo(bool);
    void setConfigMessage(ConfigMessage);

    static void GenRsa(RSA **);
    static void GenPublicKeyFromRsa(RSA *, QByteArray **);
    static void GenRsaFromPublicKey(RSA **, QByteArray *);
};


#endif // MESSAGE_H
