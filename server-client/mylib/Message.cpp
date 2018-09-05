#include "Message.h"
#include <QtDebug>


ConfigMessage::ConfigMessage(bool isCompression, bool isEncryption,
                             RSA * rsa_private, RSA * rsa_public)
    : isCompression(isCompression), isEncryption(isEncryption),
      rsa_private(rsa_private), rsa_public(rsa_public)
{}


Message::Message(ConfigMessage config, bool info) : Message(MessageType::NAME_REQUEST, nullptr, config, info)
{}

Message::Message(MessageType type, ConfigMessage config, bool info) : Message(type, nullptr, config, info)
{}

Message::Message(MessageType type, QString *data, ConfigMessage config, bool info) : config(config)
{
    this->type = type;
    this->data = data;
    isInfo = info;
}

Message::~Message()
{
    if (data != nullptr) delete data;
}

MessageType Message::getType() const
{
    return type;
}

QString * Message::getData() const
{
    return data;
}

/* Запись сообщения в поток байтов */
QByteArray * Message::writeObject()
{
    QString str;
    str += QString::number(static_cast<int>(type));
    str += '@';
    QByteArray *byteArray;
    if (data != nullptr)
    {
        QByteArray *temp = new QByteArray(data->toUtf8());
        // Шифруем сообщение, если длинное, то шифруем частями по 240 байт
        if (config.isEncryption && !isInfo)
        {
            QByteArray *temp2 = new QByteArray();
            int n = temp->size();
            int k = 0;
            while (n > 240)
            {
                QByteArray *t = new QByteArray(temp->mid(k, 240));
                encryptionMes(&t);
                *temp2 += *t;
                delete t;
                k += 240;
                n -= 240;
            }
            QByteArray *t = new QByteArray(temp->mid(k, temp->size()));
            encryptionMes(&t);
            *temp2 += *t;
            delete t;
            delete temp;
            temp = temp2;
        }
        // Сжимаем сообщение
        if (config.isCompression && !isInfo)
        {
            str += QString::number(static_cast<int>(temp->length()));
            str += '@';
            compressMes(&temp);
        }
        byteArray = new QByteArray(str.toUtf8());
        byteArray->append(*temp);
    }
    else
        byteArray = new QByteArray(str.toUtf8());
    return byteArray;
}

/* Чтение из потока байтов сообщения */
void Message::readObject(const QByteArray &qByteArray)
{
    int i = qByteArray.indexOf('@');
    if (i < 0) return;
    QString str = QString::fromUtf8(qByteArray.mid(0,i));
    type = MessageType(str.toInt());
    if (++i != qByteArray.size())
    {
        QByteArray *temp = nullptr;
        // Разжимаем сообщение
        if (config.isCompression && !isInfo)
        {
            int k = qByteArray.indexOf('@', i);
            str = QString::fromUtf8(qByteArray.mid(i,k - i));
            int len = str.toInt();

            temp = new QByteArray(qByteArray.mid(++k));
            uncompressMes(&temp, len);
        }
        // Дешифруем сообщение, если длинное, то дешифруем частями по 256 байт
        if (config.isEncryption && !isInfo)
        {
            QByteArray *temp2 = new QByteArray();
            int n = temp->size();
            int k = 0;
            while (n > 256)
            {
                QByteArray *t = new QByteArray(temp->mid(k, 256));
                unencryptionMes(&t);
                *temp2 += *t;
                delete t;
                k += 256;
                n -= 256;
            }
            QByteArray *t = new QByteArray(temp->mid(k, temp->size()));
            unencryptionMes(&t);
            *temp2 += *t;
            delete t;
            delete temp;
            temp = temp2;
        }

        if (temp != nullptr)
        {
            data = new QString(QString::fromUtf8(*temp));
            delete temp;
        }
        else
            data = new QString(QString::fromUtf8(qByteArray.mid(i)));
    }
}

/* Компрессия сообщения */
bool Message::compressMes(QByteArray **data)
{
    unsigned long srcSize = static_cast<unsigned long>((*data)->length());
    unsigned long destLen = static_cast<size_t>(12 + 1.2 * srcSize);
    unsigned char *dest = new unsigned char[destLen];
    unsigned char *src = static_cast<unsigned char*>(static_cast<void *>((*data)->data()));
    if (compress(dest, &destLen, src, srcSize) != Z_OK)
    {
        qDebug().nospace() << "Message::compress:: Fail";
        delete[] dest;
        delete src;
        return false;
    }
    delete *data;
    *data = new QByteArray(static_cast<char*>(static_cast<void *>(dest)), static_cast<int>(destLen));
    delete[] dest;
    return true;
}

/* Декомпрессия сообщения */
bool Message::uncompressMes(QByteArray **data, int destLen)
{
    unsigned long srcSize = static_cast<unsigned long>((*data)->length());
    unsigned long destSize = static_cast<unsigned long>(destLen) + 12;
    unsigned char *dest = new unsigned char[destSize];
    unsigned char *src = static_cast<unsigned char*>(static_cast<void *>((*data)->data()));
    if (uncompress(dest, &destSize, src, srcSize) != Z_OK)
    {
        qDebug().nospace() << "Message::uncompress:: Fail";
        delete[] dest;
        delete src;
        return false;
    }
    delete *data;
    *data = new QByteArray(static_cast<char*>(static_cast<void *>(dest)), destLen);
    delete[] dest;
    return true;
}

/* Шифрование публичным ключом */
bool Message::encryptionMes(QByteArray **data)
{
    int key_size = RSA_size(config.rsa_public);
    unsigned char *ctext = new unsigned char[static_cast<size_t>(key_size)];

    int inlen = (*data)->size();
    unsigned char *pt = static_cast<unsigned char*>(static_cast<void *>((*data)->data()));

    int outlen = RSA_public_encrypt(inlen, pt, ctext, config.rsa_public, RSA_PKCS1_PADDING);
    if (outlen != RSA_size(config.rsa_public))
    {
        qDebug().nospace() << "Message::encryption:: Fail";
        delete[] ctext;
        return false;
    }
    delete *data;
    *data = new QByteArray(static_cast<char *>(static_cast<void *>(ctext)), outlen);
    delete[] ctext;    
    return true;
}

/* Дешифровка приватным ключом */
bool Message::unencryptionMes(QByteArray **data)
{
    int key_size = RSA_size(config.rsa_private);
    unsigned char *ptext = new unsigned char[static_cast<size_t>(key_size)];
    int inlen = (*data)->size();
    unsigned char *ct = static_cast<unsigned char*>(static_cast<void *>((*data)->data()));
    int outlen = RSA_private_decrypt(inlen, ct, ptext, config.rsa_private, RSA_PKCS1_PADDING);
    if (outlen < 0)
    {
        qDebug().nospace() << "Message::encryption:: Fail";
        delete[] ptext;
        return false;
    }
    delete *data;
    *data = new QByteArray(static_cast<char *>(static_cast<void *>(ptext)), outlen);
    delete[] ptext;
    return true;
}


void Message::setInfo(bool info)
{
    isInfo = info;
}

void Message::setConfigMessage(ConfigMessage config)
{
    this->config = config;
}

/* Генерируем ключи в струтуре RSA */
void Message::GenRsa(RSA **rsa_private)
{
    int bits = 2048; // длина ключа в битах
    BIGNUM *e = BN_new();
    BN_set_word(e, RSA_F4);
    *rsa_private = RSA_new();
    RSA_generate_key_ex(*rsa_private, bits, e, nullptr);
    BN_free(e);
}

/* Из RSA достаем публичный ключ */
void Message::GenPublicKeyFromRsa(RSA *rsa_private, QByteArray **public_key)
{
    BIO *pub = BIO_new(BIO_s_mem());
    PEM_write_bio_RSAPublicKey(pub, rsa_private);

    int pub_len;
    pub_len = static_cast<int>(BIO_ctrl(pub, BIO_CTRL_PENDING, 0, nullptr));
    char *pub_key = new char[static_cast<size_t>(pub_len)];
    BIO_read(pub, pub_key, pub_len);
    *public_key = new QByteArray(pub_key, pub_len);

    delete[] pub_key;
    BIO_free(pub);
}

/* Из публичного ключа делаем RSA */
void Message::GenRsaFromPublicKey(RSA **rsa_public, QByteArray *public_key)
{
    int priv_len = public_key->size();
    BIO *priv = BIO_new(BIO_s_mem());
    BIO_write(priv, public_key->data(), priv_len);
    PEM_read_bio_RSAPublicKey(priv, &(*rsa_public), nullptr, nullptr);
    BIO_free(priv);
}
