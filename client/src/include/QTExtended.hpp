#include <ostream>
#include <QString>

std::ostream& operator<<(std::ostream &stream,const QString &str)
{
   return (stream << str.toStdString());
}