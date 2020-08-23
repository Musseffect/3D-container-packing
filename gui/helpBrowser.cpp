#include "helpBrowser.h"


HelpBrowser::HelpBrowser(QHelpEngine* helpEngine,
                         QWidget* parent):QTextBrowser(parent),
                         helpEngine(helpEngine)
{
}

QVariant HelpBrowser::loadResource(int type, const QUrl &name){
    if (name.scheme() == "qthelp")
        return QVariant(helpEngine->fileData(name));
    else
        return QTextBrowser::loadResource(type, name);
}
