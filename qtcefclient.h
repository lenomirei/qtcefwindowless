#ifndef QTCEFCLIENT_H
#define QTCEFCLIENT_H
#include "include/cef_client.h"

class QtCefClient : public CefClient
{
public:
    QtCefClient();

private:
    IMPLEMENT_REFCOUNTING(QtCefClient);
};

#endif // QTCEFCLIENT_H
