#include "setting.h"

setting::setting()
{

}
int setting::strColorToInt(QString colorbuffer)
{
    QString colorConst="color0,color1,black,white,darkGray,gray,lightGray,red,green,blue,cyan,magenta,yellow,darkRed,darkGreen,darkBlue,darkCyan,darkMagenta,darkYellow,transparent";
    if(colorConst.split(',').contains(colorbuffer,Qt::CaseInsensitive))
        return colorConst.split(',').indexOf(colorbuffer);
    else
        return 0;
}
QString setting::intColorToString(int colorbuffer)
{
    QString colorConst="color0,color1,black,white,darkGray,gray,lightGray,red,green,blue,cyan,magenta,yellow,darkRed,darkGreen,darkBlue,darkCyan,darkMagenta,darkYellow,transparent";
    if(colorbuffer>(colorConst.split(',').count()-1))
        return "";
    else

        return colorConst.split(',').at(colorbuffer);
}
