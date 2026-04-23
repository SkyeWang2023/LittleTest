/* vi:set ts=8 sts=4 sw=4:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * ex_getln.c: Functions for entering and editing an Ex command line.
 */

#include "vim.h"

/*
 * Variables shared between getcmdline(), redrawcmdline() and others.
 * These need to be saved when using CTRL-R |, that's why they are in a
 * structure.
 */
struct cmdline_info
{
    char_u	*cmdbuff;	/* pointer to command line buffer */
    int		cmdbufflen;	/* length of cmdbuff */
    int		cmdlen;		/* number of chars in command line */
    int		cmdpos;		/* current cursor position */
    int		cmdspos;	/* cursor column on screen */
    int		cmdfirstc;	/* ':', '/', '?', '=' or NUL */
    int		cmdindent;	/* number of spaces before cmdline */
    char_u	*cmdprompt;	/* message in front of cmdline */
    int		cmdattr;	/* attributes for prompt */
    int		overstrike;	/* Typing mode on the command line.  Shared by
				   getcmdline() and put_on_cmdline(). */
};

static struct cmdline_info ccline;

int vim_isspace(int x) {
    // 只处理 ASCII 范围内的字符
    unsigned char ch = (unsigned char)x;

    return (ch == ' '  ||  // 空格
            ch == '\t' ||  // 水平制表符
            ch == '\n' ||  // 换行
            ch == '\v' ||  // 垂直制表符
            ch == '\f' ||  // 换页
            ch == '\r');   // 回车
}

bool vim_iswordc(int c) {
    // 处理 ASCII 范围
    if (isalnum(c) || c == '_') {
        return true;
    }

    // 如果需要支持额外字符，可以在这里扩展
    // 例如：允许连字符 '-'
    // if (c == '-') return true;

    return false;
}

char_u *
getcmdline(firstc, count, indent, j)
int firstc;
long count; /* only used for incremental search */
int indent; /* indent for inside conditionals */
int j;
{
  p = ccline.cmdbuff + j;
  for (;;)
  {
    */
        switch (c)
    {
    case K_BS:
    case Ctrl_H:
    case K_DEL:
    case K_KDEL:
    case Ctrl_W:
#ifdef FEAT_FKMAP
      if (cmd_fkmap && c == K_BS)
        c = K_DEL;
#endif
      /*
       * delete current character is the same as backspace on next
       * character, except at end of line
       */
      if (c == Ctrl_W)
      {
        while (p > ccline.cmdbuff && vim_isspace(p[-1]))
          --p;
        i = vim_iswordc(p[-1]);
        while (p > ccline.cmdbuff && !vim_isspace(p[-1]) && vim_iswordc(p[-1]) == i)
          --p;
      }
    }
  }
}
