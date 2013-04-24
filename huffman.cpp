#include "huffman.h"

#include <stack>

#include <QFile>
#include <QDebug>
#include <QStringList>
#include <QRegExp>
#include <QStack>

Huffman::Huffman ()
  : root_ (NULL)
{
}

Huffman::~Huffman ()
{
  this->clear ();
}

int Huffman::addElement (const quint32 element, const QBitArray &code)
{
  /// Adds element to the huffman table.
  /// Returns 0 on success, < 0 if error.

  try
  {
    if (!root_)
    {
      root_ = new TreeNode;
    }

    TreeNode *cur = root_;
    int position = 0;

    while (position < code.size ())
    {
      if (code[position])
      {
        if (!cur->right)
        {
          cur->right = new TreeNode;
        }
        cur = cur->right;
      }
      else
      {
        if (!cur->left)
        {
          cur->left = new TreeNode;
        }
        cur = cur->left;
      }

      ++position;
    }

    // We assert that the node we arrived at is a leaf node.
    // Otherwise - error.
    if (!cur->isLeaf ())
    {
      qDebug () << QObject::tr ("Error: Inconsistent huffman table");
      return -1;
    }

    // FIXME: we could add another variable to TreeNode to check whether
    // an element was set to the node. This would allow us to detect different
    // elements assigned to a single nodes and elements assigned to non-leaf
    // nodes before they became non-leaf.

    cur->element = element;
  }
  catch (std::bad_alloc)
  {
    qDebug () << QObject::tr ("Not enough memory");
    this->clear ();
    return -2;
  }

  return 0;
}

int Huffman::readFromFile (const QString &filename)
{
  /// Reads huffman decoding table from file.
  /// Format:
  /// 30 0
  /// 888 10
  /// 999 11
  /// Returns 0 on success, <0 if error.

  QFile file (filename);

  if (!file.open (QIODevice::ReadOnly | QIODevice::Text))
  {
    qDebug () << QObject::tr ("Cannot open file") << filename;
    return -1;
  }

  int res = 0;

  while (!file.atEnd ())
  {
    QRegExp huffLineRegex ("\\s*(\\d+)\\s+([01]+)\\s*");
    if (!huffLineRegex.exactMatch (file.readLine ()))
    {
      res = -2;
      break;
    }

    QStringList line = huffLineRegex.capturedTexts ();

    bool elementOk = true;
    bool codeOk = true;
    quint32 element = line[1].toUInt (&elementOk);
    QBitArray code = str2ba (line[2], &codeOk);

    if (!elementOk || !codeOk)
    {
      res = -2;
      break;
    }

    if (this->addElement (element, code) < 0)
    {
      res = -3;
      // We are sure that table is inconsistent, so we need to clear it.
      this->clear ();
    }
  }

  if (res < 0)
  {
    qDebug () << QObject::tr ("Error reading file") << filename;
  }

  file.close ();

  return res;
}

QVector<quint32> Huffman::decode (const QBitArray &data, bool *ok) const
{
  /// Decodes data according to loaded huffman table.
  /// If error - sets ok to false and returns decoded array constructed so far.

  if (ok)
    *ok = true;

  QVector<quint32> res;
  if (root_ == NULL)
  {
    qDebug () << QObject::tr ("Error: Table is not initialized.");
    if (ok)
      *ok = false;

    return res;
  }

  TreeNode *cur = root_;

  int i = 0;
  while (i < data.size ())
  {
    if (cur == NULL)
    {
      break;
    }

    if (cur->isLeaf ())
    {
      res.push_back (cur->element);
      cur = root_;
    }
    else
    {
      cur = data[i] ? cur->right : cur->left;
      ++i;
    }
  }
  
  // If we are not in a leaf node - data was not valid
  if (cur == NULL || !cur->isLeaf ())
  {
    qDebug () << QObject::tr ("Data to decode doesn't match huffman table.");
    if (ok)
      *ok = false;
  }
  else
  {
    res.push_back (cur->element);
  }

  return res;
}

void Huffman::clear ()
{
  if (root_)
  {
    deleteTree (root_);
  }

  root_ = NULL;
}

QBitArray Huffman::str2ba (const QString &str, bool *ok)
{
  /// Converts string to bitarray.
  /// If error - sets ok to false and returns array constructed so far.

  if (ok)
    *ok = true;

  QBitArray ba (str.size ());

  for (int i = 0; i < str.size (); ++i)
  {
    if (str[i] == '1')
      ba[i] = true;
    else if (str[i] != '0')
    {
      if (ok)
        *ok = false;
      return ba;
    }
  }

  return ba;
}

QString Huffman::ba2str (const QBitArray &ba)
{
  /// Converts bitarray to string

  QString str;

  for (int i = 0; i < ba.size (); ++i)
  {
    str += ba[i] ? "1" : "0";
  }

  return str;
}

void Huffman::deleteTree (TreeNode *node)
{
  // Realization was switched from recursive to stack based to handle
  // the (unlikely) case of very large tree.
  if (node == NULL)
    return;

  QStack<TreeNode *> stack;

  stack.push (node);

  while (!stack.empty ())
  {
    TreeNode *cur = stack.pop ();
    if (cur->left)
      stack.push (cur->left);
    if (cur->right)
      stack.push (cur->right);
    delete cur;
  }

/*****************************************************
  deleteTree (node->left);
  deleteTree (node->right);
*****************************************************/
}
