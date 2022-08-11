#ifndef QTCEFAPP_H
#define QTCEFAPP_H

#include "include/cef_app.h"


class QtCefApp : public CefApp
{
public:
    QtCefApp();

private:
    IMPLEMENT_REFCOUNTING(QtCefApp);
};

#endif // QTCEFAPP_H
