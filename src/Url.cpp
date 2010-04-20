/*
  Copyright 2010 Dally Richard
  This file is part of QNetSoul.
  QNetSoul is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  QNetSoul is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with QNetSoul.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static char    *strip_return(char *str)
{
  int		cpt[2];

  for (cpt[0] = cpt[1] = 0; str[cpt[0]]; cpt[0]++, cpt[1]++)
    {
      if (str[cpt[0]] == '\\' && str[cpt[0] + 1] && str[cpt[0] + 1] == 'n')
        {
	  str[cpt[1]] = '\n';
	  cpt[0]++;
        }
      else
        {
	  str[cpt[1]] = str[cpt[0]];
        }
    }
  str[cpt[1]] = 0;
  return (str);
}

void	do_url_encode(std::string &out, const char *in)
{
  char	buf[8];

  for (unsigned int i = 0; in[i]; ++i)
    {
      if ((in[i] >= 'a' && in[i] <= 'z') ||
	  (in[i] >= 'A' && in[i] <= 'Z') ||
	  (in[i] >= '0' && in[i] <= '9') ||
	  in[i] == '_' || in[i] == '-' || in[i] == '.')
        {
	  out += (char)in[i];
        }
      else
        {
	  sprintf(buf, "%%%02X", in[i] & 0xFF);
	  out += buf;
        }
    }
}

const char*	url_encode(const char *in)
{
  static std::string	out;

  out.clear();
  do_url_encode(out, in);
  return (out.c_str());
}

const char*	url_decode(const char *in)
{
  static std::string	out;
  char					buf[8];
  char					nb[5];
  int					i;

  out.clear();
  memset(nb, 0, 5);

  for (i = 0; in[i]; ++i)
    {
      if (in[i] == '%' && in[i + 1] &&
	  ((in[i + 1] >= '0' && in[i + 1] <= '9') ||
	   (in[i + 1] >= 'A' && in[i + 1] <= 'F') ||
	   (in[i + 1] >= 'a' && in[i + 1] <= 'f')))
        {
	  sprintf(nb, "0x%.2s", in + i + 1);
	  memset(buf, 0, 8);
	  buf[0] = strtol(nb, 0, 16);
	  out += strip_return(buf);
	  i += 2;
        }
      else
        {
	  out += in[i];
        }
    }
  return (out.c_str());
}
