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

static struct cmdline_info ccline;	/* current cmdline_info */

static int	cmd_showtail;		/* Only show path tail in lists ? */

#ifdef FEAT_EVAL
static int	new_cmdpos;	/* position set by set_cmdline_pos() */
#endif

#ifdef FEAT_CMDHIST
typedef struct hist_entry
{
    int		hisnum;		/* identifying number */
    char_u	*hisstr;	/* actual entry, separator char after the NUL */
} histentry_T;

static histentry_T *(history[HIST_COUNT]) = {NULL, NULL, NULL, NULL, NULL};
static int	hisidx[HIST_COUNT] = {-1, -1, -1, -1, -1};  /* lastused entry */
static int	hisnum[HIST_COUNT] = {0, 0, 0, 0, 0};
		    /* identifying (unique) number of newest history entry */
static int	hislen = 0;		/* actual length of history tables */

static int	hist_char2type __ARGS((int c));
static void	init_history __ARGS((void));

static int	in_history __ARGS((int, char_u *, int));
# ifdef FEAT_EVAL
static int	calc_hist_idx __ARGS((int histype, int num));
# endif
#endif

#ifdef FEAT_RIGHTLEFT
static int	cmd_hkmap = 0;	/* Hebrew mapping during command line */
#endif

#ifdef FEAT_FKMAP
static int	cmd_fkmap = 0;	/* Farsi mapping during command line */
#endif

static int	cmdline_charsize __ARGS((int idx));
static void	set_cmdspos __ARGS((void));
static void	set_cmdspos_cursor __ARGS((void));
#ifdef FEAT_MBYTE
static void	correct_cmdspos __ARGS((int idx, int cells));
#endif
static void	alloc_cmdbuff __ARGS((int len));
static int	realloc_cmdbuff __ARGS((int len));
static void	draw_cmdline __ARGS((int start, int len));
#if defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
static void	redrawcmd_preedit __ARGS((void));
#endif
#ifdef FEAT_WILDMENU
static void	cmdline_del __ARGS((int from));
#endif
static void	redrawcmdprompt __ARGS((void));
static void	cursorcmd __ARGS((void));
static int	ccheck_abbr __ARGS((int));
static int	nextwild __ARGS((expand_T *xp, int type, int options));
static int	showmatches __ARGS((expand_T *xp, int wildmenu));
static void	set_expand_context __ARGS((expand_T *xp));
static int	ExpandFromContext __ARGS((expand_T *xp, char_u *, int *, char_u ***, int));
static int	expand_showtail __ARGS((expand_T *xp));
#ifdef FEAT_CMDL_COMPL
static int	ExpandRTDir __ARGS((char_u *pat, int *num_file, char_u ***file, char *dirname));
# if defined(FEAT_USR_CMDS) && defined(FEAT_EVAL)
static int	ExpandUserDefined __ARGS((expand_T *xp, regmatch_T *regmatch, int *num_file, char_u ***file));
# endif
#endif

#ifdef FEAT_CMDWIN
static int	ex_window __ARGS((void));
#endif

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


/*
 * getcmdline() - accept a command line starting with firstc.
 *
 * firstc == ':'	    get ":" command line.
 * firstc == '/' or '?'	    get search pattern
 * firstc == '='	    get expression
 * firstc == '@'	    get text for input() function
 * firstc == '>'	    get text for debug mode
 * firstc == NUL	    get text for :insert command
 * firstc == -1		    like NUL, and break on CTRL-C
 *
 * The line is collected in ccline.cmdbuff, which is reallocated to fit the
 * command line.
 *
 * Careful: getcmdline() can be called recursively!
 *
 * Return pointer to allocated string if there is a commandline, NULL
 * otherwise.
 */
/*ARGSUSED*/
    char_u *
getcmdline(firstc, count, indent)
    int		firstc;
    long	count;		/* only used for incremental search */
    int		indent;		/* indent for inside conditionals */
{
    int		c;
    int		i;
    int		j;
    int		gotesc = FALSE;		/* TRUE when <ESC> just typed */
    int		do_abbr;		/* when TRUE check for abbr. */
#ifdef FEAT_CMDHIST
    char_u	*lookfor = NULL;	/* string to match */
    int		hiscnt;			/* current history line in use */
    int		histype;		/* history type to be used */
#endif
#ifdef FEAT_SEARCH_EXTRA
    pos_T	old_cursor;
    colnr_T	old_curswant;
    colnr_T	old_leftcol;
    linenr_T	old_topline;
# ifdef FEAT_DIFF
    int		old_topfill;
# endif
    linenr_T	old_botline;
    int		did_incsearch = FALSE;
    int		incsearch_postponed = FALSE;
#endif
    int		did_wild_list = FALSE;	/* did wild_list() recently */
    int		wim_index = 0;		/* index in wim_flags[] */
    int		res;
    int		save_msg_scroll = msg_scroll;
    int		save_State = State;	/* remember State when called */
    int		some_key_typed = FALSE;	/* one of the keys was typed */
#ifdef FEAT_MOUSE
    /* mouse drag and release events are ignored, unless they are
     * preceded with a mouse down event */
    int		ignore_drag_release = TRUE;
#endif
#ifdef FEAT_EVAL
    int		break_ctrl_c = FALSE;
#endif
    expand_T	xpc;
    long	*b_im_ptr = NULL;

#ifdef FEAT_SNIFF
    want_sniff_request = 0;
#endif
#ifdef FEAT_EVAL
    if (firstc == -1)
    {
	firstc = NUL;
	break_ctrl_c = TRUE;
    }
#endif
#ifdef FEAT_RIGHTLEFT
    /* start without Hebrew mapping for a command line */
    if (firstc == ':' || firstc == '=' || firstc == '>')
	cmd_hkmap = 0;
#endif

    ccline.overstrike = FALSE;		    /* always start in insert mode */
#ifdef FEAT_SEARCH_EXTRA
    old_cursor = curwin->w_cursor;	    /* needs to be restored later */
    old_curswant = curwin->w_curswant;
    old_leftcol = curwin->w_leftcol;
    old_topline = curwin->w_topline;
# ifdef FEAT_DIFF
    old_topfill = curwin->w_topfill;
# endif
    old_botline = curwin->w_botline;
#endif

    /*
     * set some variables for redrawcmd()
     */
    ccline.cmdfirstc = (firstc == '@' ? 0 : firstc);
    ccline.cmdindent = indent;
    alloc_cmdbuff(exmode_active ? 250 : 0); /* alloc initial ccline.cmdbuff */
    if (ccline.cmdbuff == NULL)
	return NULL;			    /* out of memory */
    ccline.cmdlen = ccline.cmdpos = 0;
    ccline.cmdbuff[0] = NUL;

    ExpandInit(&xpc);

#ifdef FEAT_RIGHTLEFT
    if (curwin->w_p_rl && *curwin->w_p_rlc == 's'
					  && (firstc == '/' || firstc == '?'))
	cmdmsg_rl = TRUE;
    else
	cmdmsg_rl = FALSE;
#endif

    redir_off = TRUE;		/* don't redirect the typed command */
    if (!cmd_silent)
    {
	i = msg_scrolled;
	msg_scrolled = 0;		/* avoid wait_return message */
	gotocmdline(TRUE);
	msg_scrolled += i;
	redrawcmdprompt();		/* draw prompt or indent */
	set_cmdspos();
    }
    xpc.xp_context = EXPAND_NOTHING;
    xpc.xp_backslash = XP_BS_NONE;

    /*
     * Avoid scrolling when called by a recursive do_cmdline(), e.g. when
     * doing ":@0" when register 0 doesn't contain a CR.
     */
    msg_scroll = FALSE;

    State = CMDLINE;

    if (firstc == '/' || firstc == '?' || firstc == '@')
    {
	/* Use ":lmap" mappings for search pattern and input(). */
	if (curbuf->b_p_imsearch == B_IMODE_USE_INSERT)
	    b_im_ptr = &curbuf->b_p_iminsert;
	else
	    b_im_ptr = &curbuf->b_p_imsearch;
	if (*b_im_ptr == B_IMODE_LMAP)
	    State |= LANGMAP;
#ifdef USE_IM_CONTROL
	im_set_active(*b_im_ptr == B_IMODE_IM);
#endif
    }
#ifdef USE_IM_CONTROL
    else if (p_imcmdline)
	im_set_active(TRUE);
#endif

#ifdef FEAT_MOUSE
    setmouse();
#endif
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		/* may show different cursor shape */
#endif

#ifdef FEAT_CMDHIST
    init_history();
    hiscnt = hislen;		/* set hiscnt to impossible history value */
    histype = hist_char2type(firstc);
#endif

#ifdef FEAT_DIGRAPHS
    do_digraph(-1);		/* init digraph typahead */
#endif

    /*
     * Collect the command string, handling editing keys.
     */
    for (;;)
    {
#ifdef USE_ON_FLY_SCROLL
	dont_scroll = FALSE;	/* allow scrolling here */
#endif
	quit_more = FALSE;	/* reset after CTRL-D which had a more-prompt */

	cursorcmd();		/* set the cursor on the right spot */
	c = safe_vgetc();
	if (KeyTyped)
	{
	    some_key_typed = TRUE;
#ifdef FEAT_RIGHTLEFT
	    if (cmd_hkmap)
		c = hkmap(c);
# ifdef FEAT_FKMAP
	    if (cmd_fkmap)
		c = cmdl_fkmap(c);
# endif
	    if (cmdmsg_rl && !KeyStuffed)
	    {
		/* Invert horizontal movements and operations.  Only when
		 * typed by the user directly, not when the result of a
		 * mapping. */
		switch (c)
		{
		    case K_RIGHT:   c = K_LEFT; break;
		    case K_S_RIGHT: c = K_S_LEFT; break;
		    case K_C_RIGHT: c = K_C_LEFT; break;
		    case K_LEFT:    c = K_RIGHT; break;
		    case K_S_LEFT:  c = K_S_RIGHT; break;
		    case K_C_LEFT:  c = K_C_RIGHT; break;
		}
	    }
#endif
	}

	/*
	 * Ignore got_int when CTRL-C was typed here.
	 * Don't ignore it in :global, we really need to break then, e.g., for
	 * ":g/pat/normal /pat" (without the <CR>).
	 * Don't ignore it for the input() function.
	 */
	if ((c == Ctrl_C
#ifdef UNIX
		|| c == intr_char
#endif
				)
#if defined(FEAT_EVAL) || defined(FEAT_CRYPT)
		&& firstc != '@'
#endif
#ifdef FEAT_EVAL
		&& !break_ctrl_c
#endif
		&& !global_busy)
	    got_int = FALSE;

#ifdef FEAT_CMDHIST
	/* free old command line when finished moving around in the history
	 * list */
	if (lookfor != NULL
		&& c != K_S_DOWN && c != K_S_UP && c != K_DOWN && c != K_UP
		&& c != K_PAGEDOWN && c != K_PAGEUP
		&& c != K_KPAGEDOWN && c != K_KPAGEUP
		&& c != K_LEFT && c != K_RIGHT
		&& (xpc.xp_numfiles > 0 || (c != Ctrl_P && c != Ctrl_N)))
	{
	    vim_free(lookfor);
	    lookfor = NULL;
	}
#endif

	/*
	 * <S-Tab> works like CTRL-P (unless 'wc' is <S-Tab>).
	 */
	if (c != p_wc && c == K_S_TAB && xpc.xp_numfiles != -1)
	    c = Ctrl_P;

#ifdef FEAT_WILDMENU
	/* Special translations for 'wildmenu' */
	if (did_wild_list && p_wmnu)
	{
	    if (c == K_LEFT)
		c = Ctrl_P;
	    else if (c == K_RIGHT)
		c = Ctrl_N;
	}
	/* Hitting CR after "emenu Name.": complete submenu */
	if (xpc.xp_context == EXPAND_MENUNAMES && p_wmnu
		&& ccline.cmdpos > 1
		&& ccline.cmdbuff[ccline.cmdpos - 1] == '.'
		&& ccline.cmdbuff[ccline.cmdpos - 2] != '\\'
		&& (c == '\n' || c == '\r' || c == K_KENTER))
	    c = K_DOWN;
#endif

	/* free expanded names when finished walking through matches */
	if (xpc.xp_numfiles != -1
		&& !(c == p_wc && KeyTyped) && c != p_wcm
		&& c != Ctrl_N && c != Ctrl_P && c != Ctrl_A
		&& c != Ctrl_L)
	{
	    (void)ExpandOne(&xpc, NULL, NULL, 0, WILD_FREE);
	    did_wild_list = FALSE;
#ifdef FEAT_WILDMENU
	    if (!p_wmnu || (c != K_UP && c != K_DOWN))
#endif
		xpc.xp_context = EXPAND_NOTHING;
	    wim_index = 0;
#ifdef FEAT_WILDMENU
	    if (p_wmnu && wild_menu_showing != 0)
	    {
		int skt = KeyTyped;

		if (wild_menu_showing == WM_SCROLLED)
		{
		    /* Entered command line, move it up */
		    cmdline_row--;
		    redrawcmd();
		}
		else if (save_p_ls != -1)
		{
		    /* restore 'laststatus' and 'winminheight' */
		    p_ls = save_p_ls;
		    p_wmh = save_p_wmh;
		    last_status(FALSE);
		    update_screen(VALID);	/* redraw the screen NOW */
		    redrawcmd();
		    save_p_ls = -1;
		}
		else
		{
# ifdef FEAT_VERTSPLIT
		    win_redraw_last_status(topframe);
# else
		    lastwin->w_redr_status = TRUE;
# endif
		    redraw_statuslines();
		}
		KeyTyped = skt;
		wild_menu_showing = 0;
	    }
#endif
	}

#ifdef FEAT_WILDMENU
	/* Special translations for 'wildmenu' */
	if (xpc.xp_context == EXPAND_MENUNAMES && p_wmnu)
	{
	    /* Hitting <Down> after "emenu Name.": complete submenu */
	    if (ccline.cmdbuff[ccline.cmdpos - 1] == '.' && c == K_DOWN)
		c = p_wc;
	    else if (c == K_UP)
	    {
		/* Hitting <Up>: Remove one submenu name in front of the
		 * cursor */
		int found = FALSE;

		j = (int)(xpc.xp_pattern - ccline.cmdbuff);
		i = 0;
		while (--j > 0)
		{
		    /* check for start of menu name */
		    if (ccline.cmdbuff[j] == ' '
			    && ccline.cmdbuff[j - 1] != '\\')
		    {
			i = j + 1;
			break;
		    }
		    /* check for start of submenu name */
		    if (ccline.cmdbuff[j] == '.'
			    && ccline.cmdbuff[j - 1] != '\\')
		    {
			if (found)
			{
			    i = j + 1;
			    break;
			}
			else
			    found = TRUE;
		    }
		}
		if (i > 0)
		    cmdline_del(i);
		c = p_wc;
		xpc.xp_context = EXPAND_NOTHING;
	    }
	}
	if (xpc.xp_context == EXPAND_FILES && p_wmnu)
	{
	    char_u upseg[5];

	    upseg[0] = PATHSEP;
	    upseg[1] = '.';
	    upseg[2] = '.';
	    upseg[3] = PATHSEP;
	    upseg[4] = NUL;

	    if (ccline.cmdbuff[ccline.cmdpos - 1] == PATHSEP
		    && c == K_DOWN
		    && (ccline.cmdbuff[ccline.cmdpos - 2] != '.'
			|| ccline.cmdbuff[ccline.cmdpos - 3] != '.'))
	    {
		/* go down a directory */
		c = p_wc;
	    }
	    else if (STRNCMP(xpc.xp_pattern, upseg + 1, 3) == 0 && c == K_DOWN)
	    {
		/* If in a direct ancestor, strip off one ../ to go down */
		int found = FALSE;

		j = ccline.cmdpos;
		i = (int)(xpc.xp_pattern - ccline.cmdbuff);
		while (--j > i)
		{
		    if (vim_ispathsep(ccline.cmdbuff[j]))
		    {
			found = TRUE;
			break;
		    }
		}
		if (found
			&& ccline.cmdbuff[j - 1] == '.'
			&& ccline.cmdbuff[j - 2] == '.'
			&& (vim_ispathsep(ccline.cmdbuff[j - 3]) || j == i + 2))
		{
		    cmdline_del(j - 2);
		    c = p_wc;
		}
	    }
	    else if (c == K_UP)
	    {
		/* go up a directory */
		int found = FALSE;

		j = ccline.cmdpos - 1;
		i = (int)(xpc.xp_pattern - ccline.cmdbuff);
		while (--j > i)
		{
#ifdef FEAT_MBYTE
		    if (has_mbyte)
			j -= (*mb_head_off)(ccline.cmdbuff, ccline.cmdbuff + j);
#endif
		    if (vim_ispathsep(ccline.cmdbuff[j])
#ifdef BACKSLASH_IN_FILENAME
			    && vim_strchr(" *?[{`$%#", ccline.cmdbuff[j + 1])
			       == NULL
#endif
		       )
		    {
			if (found)
			{
			    i = j + 1;
			    break;
			}
			else
			    found = TRUE;
		    }
		}

		if (!found)
		    j = i;
		else if (STRNCMP(ccline.cmdbuff + j, upseg, 4) == 0)
		    j += 4;
		else if (STRNCMP(ccline.cmdbuff + j, upseg + 1, 3) == 0
			     && j == i)
		    j += 3;
		else
		    j = 0;
		if (j > 0)
		{
		    /* TODO this is only for DOS/UNIX systems - need to put in
		     * machine-specific stuff here and in upseg init */
		    cmdline_del(j);
		    put_on_cmdline(upseg + 1, 3, FALSE);
		}
		else if (ccline.cmdpos > i)
		    cmdline_del(i);
		c = p_wc;
	    }
	}
#if 0 /* If enabled <Down> on a file takes you _completely_ out of wildmenu */
	if (p_wmnu
		&& (xpc.xp_context == EXPAND_FILES
		    || xpc.xp_context == EXPAND_MENUNAMES)
		&& (c == K_UP || c == K_DOWN))
	    xpc.xp_context = EXPAND_NOTHING;
#endif

#endif	/* FEAT_WILDMENU */

	/* CTRL-\ CTRL-N goes to Normal mode, CTRL-\ CTRL-G goes to Insert
	 * mode when 'insertmode' is set, CTRL-\ e prompts for an expression. */
	if (c == Ctrl_BSL)
	{
	    ++no_mapping;
	    ++allow_keys;
	    c = safe_vgetc();
	    --no_mapping;
	    --allow_keys;
	    /* CTRL-\ e doesn't work when obtaining an expression. */
	    if (c != Ctrl_N && c != Ctrl_G
				     && (c != 'e' || ccline.cmdfirstc == '='))
	    {
		vungetc(c);
		c = Ctrl_BSL;
	    }
#ifdef FEAT_EVAL
	    else if (c == 'e')
	    {
		struct cmdline_info	    save_ccline;
		char_u		    *p;

		/*
		 * Replace the command line with the result of an expression.
		 * Need to save the current command line, to be able to enter
		 * a new one...
		 */
		if (ccline.cmdpos == ccline.cmdlen)
		    new_cmdpos = 99999;	/* keep it at the end */
		else
		    new_cmdpos = ccline.cmdpos;
		save_ccline = ccline;
		ccline.cmdbuff = NULL;
		ccline.cmdprompt = NULL;
		c = get_expr_register();
		ccline = save_ccline;
		if (c == '=')
		{
		    p = get_expr_line();
		    if (p != NULL
			     && realloc_cmdbuff((int)STRLEN(p) + 1) == OK)
		    {
			ccline.cmdlen = STRLEN(p);
			STRCPY(ccline.cmdbuff, p);
			vim_free(p);

			/* Restore the cursor or use the position set with
			 * set_cmdline_pos(). */
			if (new_cmdpos > ccline.cmdlen)
			    ccline.cmdpos = ccline.cmdlen;
			else
			    ccline.cmdpos = new_cmdpos;

			KeyTyped = FALSE;	/* Don't do p_wc completion. */
			redrawcmd();
			goto cmdline_changed;
		    }
		}
		beep_flush();
		c = ESC;
	    }
#endif
	    else
	    {
		if (c == Ctrl_G && p_im && restart_edit == 0)
		    restart_edit = 'a';
		gotesc = TRUE;	/* will free ccline.cmdbuff after putting it
				   in history */
		goto returncmd;	/* back to Normal mode */
	    }
	}

#ifdef FEAT_CMDWIN
	if (c == cedit_key || c == K_CMDWIN)
	{
	    /*
	     * Open a window to edit the command line (and history).
	     */
	    c = ex_window();
	    some_key_typed = TRUE;
	}
# ifdef FEAT_DIGRAPHS
	else
# endif
#endif
#ifdef FEAT_DIGRAPHS
	    c = do_digraph(c);
#endif

	if (c == '\n' || c == '\r' || c == K_KENTER || (c == ESC
			&& (!KeyTyped || vim_strchr(p_cpo, CPO_ESC) != NULL)))
	{
	    gotesc = FALSE;	/* Might have typed ESC previously, don't
				   truncate the cmdline now. */
	    if (ccheck_abbr(c + ABBR_OFF))
		goto cmdline_changed;
	    if (!cmd_silent)
	    {
		windgoto(msg_row, 0);
		out_flush();
	    }
	    break;
	}

	/*
	 * Completion for 'wildchar' or 'wildcharm' key.
	 * - hitting <ESC> twice means: abandon command line.
	 * - wildcard expansion is only done when the 'wildchar' key is really
	 *   typed, not when it comes from a macro
	 */
	if ((c == p_wc && !gotesc && KeyTyped) || c == p_wcm)
	{
	    if (xpc.xp_numfiles > 0)   /* typed p_wc at least twice */
	    {
		/* if 'wildmode' contains "list" may still need to list */
		if (xpc.xp_numfiles > 1
			&& !did_wild_list
			&& (wim_flags[wim_index] & WIM_LIST))
		{
		    (void)showmatches(&xpc, FALSE);
		    redrawcmd();
		    did_wild_list = TRUE;
		}
		if (wim_flags[wim_index] & WIM_LONGEST)
		    res = nextwild(&xpc, WILD_LONGEST, WILD_NO_BEEP);
		else if (wim_flags[wim_index] & WIM_FULL)
		    res = nextwild(&xpc, WILD_NEXT, WILD_NO_BEEP);
		else
		    res = OK;	    /* don't insert 'wildchar' now */
	    }
	    else		    /* typed p_wc first time */
	    {
		wim_index = 0;
		j = ccline.cmdpos;
		/* if 'wildmode' first contains "longest", get longest
		 * common part */
		if (wim_flags[0] & WIM_LONGEST)
		    res = nextwild(&xpc, WILD_LONGEST, WILD_NO_BEEP);
		else
		    res = nextwild(&xpc, WILD_EXPAND_KEEP, WILD_NO_BEEP);

		/* if interrupted while completing, behave like it failed */
		if (got_int)
		{
		    (void)vpeekc();	/* remove <C-C> from input stream */
		    got_int = FALSE;	/* don't abandon the command line */
		    (void)ExpandOne(&xpc, NULL, NULL, 0, WILD_FREE);
#ifdef FEAT_WILDMENU
		    xpc.xp_context = EXPAND_NOTHING;
#endif
		    goto cmdline_changed;
		}

		/* when more than one match, and 'wildmode' first contains
		 * "list", or no change and 'wildmode' contains "longest,list",
		 * list all matches */
		if (res == OK && xpc.xp_numfiles > 1)
		{
		    /* a "longest" that didn't do anything is skipped (but not
		     * "list:longest") */
		    if (wim_flags[0] == WIM_LONGEST && ccline.cmdpos == j)
			wim_index = 1;
		    if ((wim_flags[wim_index] & WIM_LIST)
#ifdef FEAT_WILDMENU
			    || (p_wmnu && (wim_flags[wim_index] & WIM_FULL) != 0)
#endif
			    )
		    {
			if (!(wim_flags[0] & WIM_LONGEST))
			{
#ifdef FEAT_WILDMENU
			    int p_wmnu_save = p_wmnu;
			    p_wmnu = 0;
#endif
			    nextwild(&xpc, WILD_PREV, 0); /* remove match */
#ifdef FEAT_WILDMENU
			    p_wmnu = p_wmnu_save;
#endif
			}
#ifdef FEAT_WILDMENU
			(void)showmatches(&xpc, p_wmnu
				&& ((wim_flags[wim_index] & WIM_LIST) == 0));
#else
			(void)showmatches(&xpc, FALSE);
#endif
			redrawcmd();
			did_wild_list = TRUE;
			if (wim_flags[wim_index] & WIM_LONGEST)
			    nextwild(&xpc, WILD_LONGEST, WILD_NO_BEEP);
			else if (wim_flags[wim_index] & WIM_FULL)
			    nextwild(&xpc, WILD_NEXT, WILD_NO_BEEP);
		    }
		    else
			vim_beep();
		}
#ifdef FEAT_WILDMENU
		else if (xpc.xp_numfiles == -1)
		    xpc.xp_context = EXPAND_NOTHING;
#endif
	    }
	    if (wim_index < 3)
		++wim_index;
	    if (c == ESC)
		gotesc = TRUE;
	    if (res == OK)
		goto cmdline_changed;
	}

	gotesc = FALSE;

	/* <S-Tab> goes to last match, in a clumsy way */
	if (c == K_S_TAB && KeyTyped)
	{
	    if (nextwild(&xpc, WILD_EXPAND_KEEP, 0) == OK
		    && nextwild(&xpc, WILD_PREV, 0) == OK
		    && nextwild(&xpc, WILD_PREV, 0) == OK)
		goto cmdline_changed;
	}

	if (c == NUL || c == K_ZERO)	    /* NUL is stored as NL */
	    c = NL;

	do_abbr = TRUE;		/* default: check for abbreviation */

	/*
	 * Big switch for a typed command line character.
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
		if (c == K_KDEL)
		    c = K_DEL;

		/*
		 * delete current character is the same as backspace on next
		 * character, except at end of line
		 */
		if (c == K_DEL && ccline.cmdpos != ccline.cmdlen)
		    ++ccline.cmdpos;
#ifdef FEAT_MBYTE
		if (has_mbyte && c == K_DEL)
		    ccline.cmdpos += mb_off_next(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
#endif
		if (ccline.cmdpos > 0)
		{
		    char_u *p;

		    j = ccline.cmdpos;
		    p = ccline.cmdbuff + j;
#ifdef FEAT_MBYTE
		    if (has_mbyte)
		    {
			p = mb_prevptr(ccline.cmdbuff, p);
			if (c == Ctrl_W)
			{
			    while (p > ccline.cmdbuff && vim_isspace(*p))
				p = mb_prevptr(ccline.cmdbuff, p);
			    i = mb_get_class(p);
			    while (p > ccline.cmdbuff && mb_get_class(p) == i)
				p = mb_prevptr(ccline.cmdbuff, p);
			    if (mb_get_class(p) != i)
				p += (*mb_ptr2len_check)(p);
			}
		    }
		    else
#endif
			if (c == Ctrl_W)
		    {
			while (p > ccline.cmdbuff && vim_isspace(p[-1]))
			    --p;
			i = vim_iswordc(p[-1]);
			while (p > ccline.cmdbuff && !vim_isspace(p[-1])
				&& vim_iswordc(p[-1]) == i)
			    --p;
		    }
		    else
			--p;
		    ccline.cmdpos = (int)(p - ccline.cmdbuff);
		    ccline.cmdlen -= j - ccline.cmdpos;
		    i = ccline.cmdpos;
		    while (i < ccline.cmdlen)
			ccline.cmdbuff[i++] = ccline.cmdbuff[j++];

		    /* Truncate at the end, required for multi-byte chars. */
		    ccline.cmdbuff[ccline.cmdlen] = NUL;
		    redrawcmd();
    }

/*
 * This part implements incremental searches for "/" and "?"
 * Jump to cmdline_not_changed when a character has been read but the command
 * line did not change. Then we only search and redraw if something changed in
 * the past.
 * Jump to cmdline_changed when the command line did change.
 * (Sorry for the goto's, I know it is ugly).
 */
cmdline_not_changed:
#ifdef FEAT_SEARCH_EXTRA
	if (!incsearch_postponed)
	    continue;
#endif

cmdline_changed:
  return ccline.cmdbuff;
}
