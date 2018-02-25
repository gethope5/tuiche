#include "speak.h"
Speak::Speak(QObject *parent) :
    strSpeak(""),
    QThread(parent)
{
//    strSpeak="speak";
    start();
}
void Speak::run(void)
{
//    qDebug()<<"another thread";
    wchar_t* speek_str = new wchar_t[strSpeak.length()+1];
    if(strSpeak.toWCharArray(speek_str))
    {
        Speek((wchar_t*)strSpeak.utf16());
    }
}
