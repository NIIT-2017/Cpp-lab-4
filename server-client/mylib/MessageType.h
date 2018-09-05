#ifndef MESSAGETYPE_H
#define MESSAGETYPE_H


enum class MessageType : int {
    SIZE_DATA = 0,
    DATA = 1,
    READY = 2,
    NAME_REQUEST = 3,
    NAME_ACCEPTED = 4,
    USER_NAME = 5,
    CONFIG = 6
};


#endif // MESSAGETYPE_H
