#include <string>
#include <iostream>
#include <algorithm>
#include "Encryption.h"

namespace
{
  const std::string ascii_table =
    "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

  const std::string code =
    "}T,f9D2E;SdHt#lY\\3AI|wZ-/K<_`[?o.e$xha*7cq5pzrj!OJy=UL1]R(ui>^'NVm)BF0:Xg@sPQ+{C8kn4vM~Wb\"G&%6";
}

// tester avec "

//void	encryption_init(void) __attribute__ ((constructor));

/*
void	encryption_init(void)
{
  //random_shuffle(code.begin(), code.end());
  qDebug() << code;
  //return ;
}
*/

QString		encrypt(const QString& msg)
{
  std::string	std_msg = msg.toStdString();
  const int	size = std_msg.length();

  for (int i = 0; i < size; ++i)
    std_msg[i] = code[std_msg[i] - 33];

  return msg.fromStdString(std_msg);
}

QString		unencrypt(const QString& msg)
{
  std::string	std_msg = msg.toStdString();
  const int	size = std_msg.length();

  for (int i = 0; i < size; ++i)
    for (int j = 0; j < 94; ++j)
      if (code[j] == std_msg[i])
	{
	  std_msg[i] = ascii_table[j];
	  break;
	}
  return msg.fromStdString(std_msg);
}
