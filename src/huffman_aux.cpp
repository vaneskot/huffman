#include "huffman_aux.h"

#include <QFile>
#include <QRegExp>
#include <QStringList>

namespace Huffman
{
Code str2code (const QString &str, bool *ok)
{
  /// Converts string to bitarray.
  /// If error - sets ok to false and returns array constructed so far.

  if (ok)
    *ok = true;

  Code code (str.size ());

  for (int i = 0; i < str.size (); ++i)
  {
    if (str[i] == '1')
      code[i] = true;
    else if (str[i] != '0')
    {
      if (ok)
        *ok = false;
      return code;
    }
  }

  return code;
}

QString code2str (const Code &code)
{
  /// Converts bitarray to string

  QString str;

  for (int i = 0; i < code.size (); ++i)
  {
    str += code[i] ? "1" : "0";
  }

  return str;
}

int readFromFile (const QString &filename, HuffmanTable<quint32> *huffman)
{
  /// Reads huffman decoding table from file.
  /// Format:
  /// 30 0
  /// 888 10
  /// 999 11
  /// Returns 0 on success, <0 if error.

  Q_ASSERT (huffman != NULL);

  if (huffman == NULL)
    return -1;

  QFile file (filename);

  if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug () << QObject::tr ("Cannot open file") << filename;
    return -2;
  }

  int res = 0;

  while (!file.atEnd ())
  {
    QRegExp huffLineRegex ("\\s*(\\d+)\\s+([01]+)\\s*");
    if (!huffLineRegex.exactMatch (file.readLine ()))
    {
      res = -3;
      break;
    }

    QStringList line = huffLineRegex.capturedTexts ();

    bool elementOk = true;
    bool codeOk = true;
    quint32 element = line[1].toUInt (&elementOk);
    Code code = Huffman::str2code (line[2], &codeOk);

    if (!elementOk || !codeOk)
    {
      res = -3;
      break;
    }

    if (huffman->addElement (element, code) < 0)
    {
      res = -3;
      // We are sure that table is inconsistent, so we need to clear it.
      huffman->clear ();
    }
  }

  if (res < 0)
  {
    qDebug () << QObject::tr ("Error reading file") << filename;
  }

  file.close ();

  return res;
}

}
