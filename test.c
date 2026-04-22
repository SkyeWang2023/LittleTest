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
		else if (ccline.cmdlen == 0 && c != Ctrl_W
				   && ccline.cmdprompt == NULL && indent == 0)
		{
		    /* In ex and debug mode it doesn't make sense to return. */
		    if (exmode_active
#ifdef FEAT_EVAL
			    || ccline.cmdfirstc == '>'
#endif
			    )
			goto cmdline_not_changed;

		    vim_free(ccline.cmdbuff);	/* no commandline to return */
		    ccline.cmdbuff = NULL;
		    if (!cmd_silent)
		    {
#ifdef FEAT_RIGHTLEFT
			if (cmdmsg_rl)
			    msg_col = Columns;
			else
#endif
			    msg_col = 0;
			msg_putchar(' ');		/* delete ':' */
		    }
		    redraw_cmdline = TRUE;
		    goto returncmd;		/* back to cmd mode */
		}
		goto cmdline_changed;

	case K_INS:
	case K_KINS:
#ifdef FEAT_FKMAP
		/* if Farsi mode set, we are in reverse insert mode -
		   Do not change the mode */
		if (cmd_fkmap)
		    beep_flush();
		else
#endif
		ccline.overstrike = !ccline.overstrike;
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	/* may show different cursor shape */
#endif
		goto cmdline_not_changed;

	case Ctrl_HAT:
		if (map_to_exists_mode((char_u *)"", LANGMAP))
		{
		    /* ":lmap" mappings exists, toggle use of mappings. */
		    State ^= LANGMAP;
#ifdef USE_IM_CONTROL
		    im_set_active(FALSE);	/* Disable input method */
#endif
		    if (b_im_ptr != NULL)
		    {
			if (State & LANGMAP)
			    *b_im_ptr = B_IMODE_LMAP;
			else
			    *b_im_ptr = B_IMODE_NONE;
		    }
		}
#ifdef USE_IM_CONTROL
		else
		{
		    /* There are no ":lmap" mappings, toggle IM.  When
		     * 'imdisable' is set don't try getting the status, it's
		     * always off. */
		    if ((p_imdisable && b_im_ptr != NULL)
			    ? *b_im_ptr == B_IMODE_IM : im_get_status())
		    {
			im_set_active(FALSE);	/* Disable input method */
			if (b_im_ptr != NULL)
			    *b_im_ptr = B_IMODE_NONE;
		    }
		    else
		    {
			im_set_active(TRUE);	/* Enable input method */
			if (b_im_ptr != NULL)
			    *b_im_ptr = B_IMODE_IM;
		    }
		}
#endif
		if (b_im_ptr != NULL)
		{
		    if (b_im_ptr == &curbuf->b_p_iminsert)
			set_iminsert_global();
		    else
			set_imsearch_global();
		}
#ifdef CURSOR_SHAPE
		ui_cursor_shape();	/* may show different cursor shape */
#endif
#if defined(FEAT_WINDOWS) && defined(FEAT_KEYMAP)
		/* Show/unshow value of 'keymap' in status lines later. */
		status_redraw_curbuf();
#endif
		goto cmdline_not_changed;

/*	case '@':   only in very old vi */
	case Ctrl_U:
		/* delete all characters left of the cursor */
		j = ccline.cmdpos;
		ccline.cmdlen -= j;
		i = ccline.cmdpos = 0;
		while (i < ccline.cmdlen)
		    ccline.cmdbuff[i++] = ccline.cmdbuff[j++];
		/* Truncate at the end, required for multi-byte chars. */
		ccline.cmdbuff[ccline.cmdlen] = NUL;
		redrawcmd();
		goto cmdline_changed;

#ifdef FEAT_CLIPBOARD
	case Ctrl_Y:
		/* Copy the modeless selection, if there is one. */
		if (clip_star.state != SELECT_CLEARED)
		{
		    if (clip_star.state == SELECT_DONE)
			clip_copy_modeless_selection(TRUE);
		    goto cmdline_not_changed;
		}
		break;
#endif

	case ESC:	/* get here if p_wc != ESC or when ESC typed twice */
	case Ctrl_C:
		/* In exmode it doesn't make sense to return. */
		if (exmode_active)
		    goto cmdline_not_changed;

		gotesc = TRUE;		/* will free ccline.cmdbuff after
					   putting it in history */
		goto returncmd;		/* back to cmd mode */

	case Ctrl_R:			/* insert register */
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	/* disallow scrolling here */
#endif
		putcmdline('"', TRUE);
		++no_mapping;
		i = c = safe_vgetc();	/* CTRL-R <char> */
		if (i == Ctrl_O)
		    i = Ctrl_R;		/* CTRL-R CTRL-O == CTRL-R CTRL-R */
		if (i == Ctrl_R)
		    c = safe_vgetc();	/* CTRL-R CTRL-R <char> */
		--no_mapping;
#ifdef FEAT_EVAL
		/*
		 * Insert the result of an expression.
		 * Need to save the current command line, to be able to enter
		 * a new one...
		 */
		new_cmdpos = -1;
		if (c == '=')
		{
		    struct cmdline_info	    save_ccline;

		    if (ccline.cmdfirstc == '=')/* can't do this recursively */
		    {
			beep_flush();
			c = ESC;
		    }
		    else
		    {
			save_ccline = ccline;
			ccline.cmdbuff = NULL;
			ccline.cmdprompt = NULL;
			c = get_expr_register();
			ccline = save_ccline;
		    }
		}
#endif
		if (c != ESC)	    /* use ESC to cancel inserting register */
		{
		    cmdline_paste(c, i == Ctrl_R);
		    KeyTyped = FALSE;	/* Don't do p_wc completion. */
#ifdef FEAT_EVAL
		    if (new_cmdpos >= 0)
		    {
			/* set_cmdline_pos() was used */
			if (new_cmdpos > ccline.cmdlen)
			    ccline.cmdpos = ccline.cmdlen;
			else
			    ccline.cmdpos = new_cmdpos;
		    }
#endif
		}
		redrawcmd();
		goto cmdline_changed;

	case Ctrl_D:
		if (showmatches(&xpc, FALSE) == EXPAND_NOTHING)
		    break;	/* Use ^D as normal char instead */

		redrawcmd();
		continue;	/* don't do incremental search now */

	case K_RIGHT:
	case K_S_RIGHT:
	case K_C_RIGHT:
		do
		{
		    if (ccline.cmdpos >= ccline.cmdlen)
			break;
		    i = cmdline_charsize(ccline.cmdpos);
		    if (KeyTyped && ccline.cmdspos + i >= Columns * Rows)
			break;
		    ccline.cmdspos += i;
#ifdef FEAT_MBYTE
		    if (has_mbyte)
			ccline.cmdpos += (*mb_ptr2len_check)(ccline.cmdbuff
							     + ccline.cmdpos);
		    else
#endif
			++ccline.cmdpos;
		}
		while ((c == K_S_RIGHT || c == K_C_RIGHT)
			&& ccline.cmdbuff[ccline.cmdpos] != ' ');
#ifdef FEAT_MBYTE
		if (has_mbyte)
		    set_cmdspos_cursor();
#endif
		goto cmdline_not_changed;

	case K_LEFT:
	case K_S_LEFT:
	case K_C_LEFT:
		do
		{
		    if (ccline.cmdpos == 0)
			break;
		    --ccline.cmdpos;
#ifdef FEAT_MBYTE
		    if (has_mbyte)	/* move to first byte of char */
			ccline.cmdpos -= (*mb_head_off)(ccline.cmdbuff,
					      ccline.cmdbuff + ccline.cmdpos);
#endif
		    ccline.cmdspos -= cmdline_charsize(ccline.cmdpos);
		}
		while ((c == K_S_LEFT || c == K_C_LEFT)
			&& ccline.cmdbuff[ccline.cmdpos - 1] != ' ');
#ifdef FEAT_MBYTE
		if (has_mbyte)
		    set_cmdspos_cursor();
#endif
		goto cmdline_not_changed;

	case K_IGNORE:
		goto cmdline_not_changed;	/* Ignore mouse */

#ifdef FEAT_MOUSE
	case K_MIDDLEDRAG:
	case K_MIDDLERELEASE:
		goto cmdline_not_changed;	/* Ignore mouse */

	case K_MIDDLEMOUSE:
# ifdef FEAT_GUI
		/* When GUI is active, also paste when 'mouse' is empty */
		if (!gui.in_use)
# endif
		    if (!mouse_has(MOUSE_COMMAND))
			goto cmdline_not_changed;   /* Ignore mouse */
#ifdef FEAT_CLIPBOARD
		if (clip_star.available)
		    cmdline_paste('*', TRUE);
		else
#endif
		    cmdline_paste(0, TRUE);
		redrawcmd();
		goto cmdline_changed;

#ifdef FEAT_DND
	case K_DROP:
		cmdline_paste('~', TRUE);
		redrawcmd();
		goto cmdline_changed;
#endif

	case K_LEFTDRAG:
	case K_LEFTRELEASE:
	case K_RIGHTDRAG:
	case K_RIGHTRELEASE:
		/* Ignore drag and release events when the button-down wasn't
		 * seen before. */
		if (ignore_drag_release)
		    goto cmdline_not_changed;
		/* FALLTHROUGH */
	case K_LEFTMOUSE:
	case K_RIGHTMOUSE:
		if (c == K_LEFTRELEASE || c == K_RIGHTRELEASE)
		    ignore_drag_release = TRUE;
		else
		    ignore_drag_release = FALSE;
# ifdef FEAT_GUI
		/* When GUI is active, also move when 'mouse' is empty */
		if (!gui.in_use)
# endif
		    if (!mouse_has(MOUSE_COMMAND))
			goto cmdline_not_changed;   /* Ignore mouse */
# ifdef FEAT_CLIPBOARD
		if (mouse_row < cmdline_row && clip_star.available)
		{
		    int	    button, is_click, is_drag;

		    /*
		     * Handle modeless selection.
		     */
		    button = get_mouse_button(KEY2TERMCAP1(c),
							 &is_click, &is_drag);
		    if (mouse_model_popup() && button == MOUSE_LEFT
					       && (mod_mask & MOD_MASK_SHIFT))
		    {
			/* Translate shift-left to right button. */
			button = MOUSE_RIGHT;
			mod_mask &= ~MOD_MASK_SHIFT;
		    }
		    clip_modeless(button, is_click, is_drag);
		    goto cmdline_not_changed;
		}
# endif

		set_cmdspos();
		for (ccline.cmdpos = 0; ccline.cmdpos < ccline.cmdlen;
							      ++ccline.cmdpos)
		{
		    i = cmdline_charsize(ccline.cmdpos);
		    if (mouse_row <= cmdline_row + ccline.cmdspos / Columns
				  && mouse_col < ccline.cmdspos % Columns + i)
			break;
#ifdef FEAT_MBYTE
		    if (has_mbyte)
		    {
			/* Count ">" for double-wide char that doesn't fit. */
			correct_cmdspos(ccline.cmdpos, i);
			ccline.cmdpos += (*mb_ptr2len_check)(ccline.cmdbuff
							 + ccline.cmdpos) - 1;
		    }
#endif
		    ccline.cmdspos += i;
		}
		goto cmdline_not_changed;

	/* Mouse scroll wheel: ignored here */
	case K_MOUSEDOWN:
	case K_MOUSEUP:
	/* Alternate buttons ignored here */
	case K_X1MOUSE:
	case K_X1DRAG:
	case K_X1RELEASE:
	case K_X2MOUSE:
	case K_X2DRAG:
	case K_X2RELEASE:
		goto cmdline_not_changed;

#endif	/* FEAT_MOUSE */

#ifdef FEAT_GUI
	case K_LEFTMOUSE_NM:	/* mousefocus click, ignored */
	case K_LEFTRELEASE_NM:
		goto cmdline_not_changed;

	case K_VER_SCROLLBAR:
		if (!msg_scrolled)
		{
		    gui_do_scroll();
		    redrawcmd();
		}
		goto cmdline_not_changed;

	case K_HOR_SCROLLBAR:
		if (!msg_scrolled)
		{
		    gui_do_horiz_scroll();
		    redrawcmd();
		}
		goto cmdline_not_changed;
#endif
	case K_SELECT:	    /* end of Select mode mapping - ignore */
		goto cmdline_not_changed;

	case Ctrl_B:	    /* begin of command line */
	case K_HOME:
	case K_KHOME:
	case K_XHOME:
	case K_S_HOME:
	case K_C_HOME:
		ccline.cmdpos = 0;
		set_cmdspos();
		goto cmdline_not_changed;

	case Ctrl_E:	    /* end of command line */
	case K_END:
	case K_KEND:
	case K_XEND:
	case K_S_END:
	case K_C_END:
		ccline.cmdpos = ccline.cmdlen;
		set_cmdspos_cursor();
		goto cmdline_not_changed;

	case Ctrl_A:	    /* all matches */
		if (nextwild(&xpc, WILD_ALL, 0) == FAIL)
		    break;
		goto cmdline_changed;

	case Ctrl_L:	    /* longest common part */
		if (nextwild(&xpc, WILD_LONGEST, 0) == FAIL)
		    break;
		goto cmdline_changed;

	case Ctrl_N:	    /* next match */
	case Ctrl_P:	    /* previous match */
		if (xpc.xp_numfiles > 0)
		{
		    if (nextwild(&xpc, (c == Ctrl_P) ? WILD_PREV : WILD_NEXT, 0)
								      == FAIL)
			break;
		    goto cmdline_changed;
		}

#ifdef FEAT_CMDHIST
	case K_UP:
	case K_DOWN:
	case K_S_UP:
	case K_S_DOWN:
	case K_PAGEUP:
	case K_KPAGEUP:
	case K_PAGEDOWN:
	case K_KPAGEDOWN:
		if (hislen == 0 || firstc == NUL)	/* no history */
		    goto cmdline_not_changed;

		i = hiscnt;

		/* save current command string so it can be restored later */
		if (lookfor == NULL)
		{
		    if ((lookfor = vim_strsave(ccline.cmdbuff)) == NULL)
			goto cmdline_not_changed;
		    lookfor[ccline.cmdpos] = NUL;
		}

		j = (int)STRLEN(lookfor);
		for (;;)
		{
		    /* one step backwards */
		    if (c == K_UP || c == K_S_UP || c == Ctrl_P ||
			    c == K_PAGEUP || c == K_KPAGEUP)
		    {
			if (hiscnt == hislen)	/* first time */
			    hiscnt = hisidx[histype];
			else if (hiscnt == 0 && hisidx[histype] != hislen - 1)
			    hiscnt = hislen - 1;
			else if (hiscnt != hisidx[histype] + 1)
			    --hiscnt;
			else			/* at top of list */
			{
			    hiscnt = i;
			    break;
			}
		    }
		    else    /* one step forwards */
		    {
			/* on last entry, clear the line */
			if (hiscnt == hisidx[histype])
			{
			    hiscnt = hislen;
			    break;
			}

			/* not on a history line, nothing to do */
			if (hiscnt == hislen)
			    break;
			if (hiscnt == hislen - 1)   /* wrap around */
			    hiscnt = 0;
			else
			    ++hiscnt;
		    }
		    if (hiscnt < 0 || history[histype][hiscnt].hisstr == NULL)
		    {
			hiscnt = i;
			break;
		    }
		    if ((c != K_UP && c != K_DOWN) || hiscnt == i
			    || STRNCMP(history[histype][hiscnt].hisstr,
						    lookfor, (size_t)j) == 0)
			break;
		}

		if (hiscnt != i)	/* jumped to other entry */
		{
		    char_u	*p;
		    int		len;
		    int		old_firstc;

		    vim_free(ccline.cmdbuff);
		    if (hiscnt == hislen)
			p = lookfor;	/* back to the old one */
		    else
			p = history[histype][hiscnt].hisstr;

		    if (histype == HIST_SEARCH
			    && p != lookfor
			    && (old_firstc = p[STRLEN(p) + 1]) != firstc)
		    {
			/* Correct for the separator character used when
			 * adding the history entry vs the one used now.
			 * First loop: count length.
			 * Second loop: copy the characters. */
			for (i = 0; i <= 1; ++i)
			{
			    len = 0;
			    for (j = 0; p[j] != NUL; ++j)
			    {
				/* Replace old sep with new sep, unless it is
				 * escaped. */
				if (p[j] == old_firstc
					      && (j == 0 || p[j - 1] != '\\'))
				{
				    if (i > 0)
					ccline.cmdbuff[len] = firstc;
				}
				else
				{
				    /* Escape new sep, unless it is already
				     * escaped. */
				    if (p[j] == firstc
					      && (j == 0 || p[j - 1] != '\\'))
				    {
					if (i > 0)
					    ccline.cmdbuff[len] = '\\';
					++len;
				    }
				    if (i > 0)
					ccline.cmdbuff[len] = p[j];
				}
				++len;
			    }
			    if (i == 0)
			    {
				alloc_cmdbuff(len);
				if (ccline.cmdbuff == NULL)
				    goto returncmd;
			    }
			}
			ccline.cmdbuff[len] = NUL;
		    }
		    else
		    {
			alloc_cmdbuff((int)STRLEN(p));
			if (ccline.cmdbuff == NULL)
			    goto returncmd;
			STRCPY(ccline.cmdbuff, p);
		    }

		    ccline.cmdpos = ccline.cmdlen = (int)STRLEN(ccline.cmdbuff);
		    redrawcmd();
		    goto cmdline_changed;
		}
		beep_flush();
		goto cmdline_not_changed;
#endif

	case Ctrl_V:
	case Ctrl_Q:
#ifdef FEAT_MOUSE
		ignore_drag_release = TRUE;
#endif
		putcmdline('^', TRUE);
		c = get_literal();	    /* get next (two) character(s) */
		do_abbr = FALSE;	    /* don't do abbreviation now */
#ifdef FEAT_MBYTE
		/* may need to remove ^ when composing char was typed */
		if (enc_utf8 && utf_iscomposing(c) && !cmd_silent)
		{
		    draw_cmdline(ccline.cmdpos, ccline.cmdlen - ccline.cmdpos);
		    msg_putchar(' ');
		    cursorcmd();
		}
#endif
		break;

#ifdef FEAT_DIGRAPHS
	case Ctrl_K:
#ifdef FEAT_MOUSE
		ignore_drag_release = TRUE;
#endif
		putcmdline('?', TRUE);
#ifdef USE_ON_FLY_SCROLL
		dont_scroll = TRUE;	    /* disallow scrolling here */
#endif
		c = get_digraph(TRUE);
		if (c != NUL)
		    break;

		redrawcmd();
		goto cmdline_not_changed;
#endif /* FEAT_DIGRAPHS */

#ifdef FEAT_RIGHTLEFT
	case Ctrl__:	    /* CTRL-_: switch language mode */
		if (!p_ari)
		    break;
#ifdef FEAT_FKMAP
		if (p_altkeymap)
		{
		    cmd_fkmap = !cmd_fkmap;
		    if (cmd_fkmap)	/* in Farsi always in Insert mode */
			ccline.overstrike = FALSE;
		}
		else			    /* Hebrew is default */
#endif
		    cmd_hkmap = !cmd_hkmap;
		goto cmdline_not_changed;
#endif

	default:
#ifdef UNIX
		if (c == intr_char)
		{
		    gotesc = TRUE;	/* will free ccline.cmdbuff after
					   putting it in history */
		    goto returncmd;	/* back to Normal mode */
		}
#endif
		/*
		 * Normal character with no special meaning.  Just set mod_mask
		 * to 0x0 so that typing Shift-Space in the GUI doesn't enter
		 * the string <S-Space>.  This should only happen after ^V.
		 */
		if (!IS_SPECIAL(c))
		    mod_mask = 0x0;
		break;
	}
	/*
	 * End of switch on command line character.
	 * We come here if we have a normal character.
	 */

	if (do_abbr && (IS_SPECIAL(c) || !vim_iswordc(c)) && ccheck_abbr(
#ifdef FEAT_MBYTE
			/* Add ABBR_OFF for characters above 0x100, this is
			 * what check_abbr() expects. */
			(has_mbyte && c >= 0x100) ? (c + ABBR_OFF) :
#endif
									c))
	    goto cmdline_changed;

	/*
	 * put the character in the command line
	 */
	if (IS_SPECIAL(c) || mod_mask != 0)
	    put_on_cmdline(get_special_key_name(c, mod_mask), -1, TRUE);
	else
	{
#ifdef FEAT_MBYTE
	    if (has_mbyte)
	    {
		j = (*mb_char2bytes)(c, IObuff);
		IObuff[j] = NUL;	/* exclude composing chars */
		put_on_cmdline(IObuff, j, TRUE);
	    }
	    else
#endif
	    {
		IObuff[0] = c;
		put_on_cmdline(IObuff, 1, TRUE);
	    }
	}
	goto cmdline_changed;

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
#ifdef FEAT_SEARCH_EXTRA
	/*
	 * 'incsearch' highlighting.
	 */
	if (p_is && !cmd_silent && (firstc == '/' || firstc == '?'))
	{
	    /* if there is a character waiting, search and redraw later */
	    if (char_avail())
	    {
		incsearch_postponed = TRUE;
		continue;
	    }
	    incsearch_postponed = FALSE;
	    curwin->w_cursor = old_cursor;  /* start at old position */

	    /* If there is no command line, don't do anything */
	    if (ccline.cmdlen == 0)
		i = 0;
	    else
	    {
		cursor_off();		/* so the user knows we're busy */
		out_flush();
		++emsg_off;    /* So it doesn't beep if bad expr */
		i = do_search(NULL, firstc, ccline.cmdbuff, count,
			SEARCH_KEEP + SEARCH_OPT + SEARCH_NOOF + SEARCH_PEEK);
		--emsg_off;
		/* if interrupted while searching, behave like it failed */
		if (got_int)
		{
		    (void)vpeekc();	/* remove <C-C> from input stream */
		    got_int = FALSE;	/* don't abandon the command line */
		    i = 0;
		}
		else if (char_avail())
		    /* cancelled searching because a char was typed */
		    incsearch_postponed = TRUE;
	    }
	    if (i)
		highlight_match = TRUE;		/* highlight position */
	    else
		highlight_match = FALSE;	/* remove highlight */

	    /* first restore the old curwin values, so the screen is
	     * positioned in the same way as the actual search command */
	    curwin->w_leftcol = old_leftcol;
	    curwin->w_topline = old_topline;
# ifdef FEAT_DIFF
	    curwin->w_topfill = old_topfill;
# endif
	    curwin->w_botline = old_botline;
	    changed_cline_bef_curs();
	    update_topline();

	    if (i != 0)
	    {
		/*
		 * First move cursor to end of match, then to start.  This
		 * moves the whole match onto the screen when 'nowrap' is set.
		 */
		i = curwin->w_cursor.col;
		curwin->w_cursor.lnum += search_match_lines;
		curwin->w_cursor.col = search_match_endcol;
		validate_cursor();
		curwin->w_cursor.lnum -= search_match_lines;
		curwin->w_cursor.col = i;
	    }
	    validate_cursor();

	    update_screen(NOT_VALID);
	    msg_starthere();
	    redrawcmdline();
	    did_incsearch = TRUE;
	}
#else /* FEAT_SEARCH_EXTRA */
	;
#endif

#ifdef FEAT_RIGHTLEFT
	if (cmdmsg_rl
# ifdef FEAT_ARABIC
		|| p_arshape
# endif
		)
	    /* Always redraw the whole command line to fix shaping and
	     * right-left typing.  Not efficient, but it works. */
	    redrawcmd();
#endif
    }

returncmd:

#ifdef FEAT_RIGHTLEFT
    cmdmsg_rl = FALSE;
#endif

#ifdef FEAT_FKMAP
    cmd_fkmap = 0;
#endif

    ExpandCleanup(&xpc);

#ifdef FEAT_SEARCH_EXTRA
    if (did_incsearch)
    {
	curwin->w_cursor = old_cursor;
	curwin->w_curswant = old_curswant;
	curwin->w_leftcol = old_leftcol;
	curwin->w_topline = old_topline;
# ifdef FEAT_DIFF
	curwin->w_topfill = old_topfill;
# endif
	curwin->w_botline = old_botline;
	highlight_match = FALSE;
	validate_cursor();	/* needed for TAB */
	redraw_later(NOT_VALID);
    }
#endif

    if (ccline.cmdbuff != NULL)
    {
	/*
	 * Put line in history buffer (":" and "=" only when it was typed).
	 */
#ifdef FEAT_CMDHIST
	if (ccline.cmdlen && firstc != NUL
		&& (some_key_typed || histype == HIST_SEARCH))
	{
	    add_to_history(histype, ccline.cmdbuff, TRUE,
				       histype == HIST_SEARCH ? firstc : NUL);
	    if (firstc == ':')
	    {
		vim_free(new_last_cmdline);
		new_last_cmdline = vim_strsave(ccline.cmdbuff);
	    }
	}
#endif

	if (gotesc)	    /* abandon command line */
	{
	    vim_free(ccline.cmdbuff);
	    ccline.cmdbuff = NULL;
	    if (msg_scrolled == 0)
		compute_cmdrow();
	    MSG("");
	    redraw_cmdline = TRUE;
	}
    }

    /*
     * If the screen was shifted up, redraw the whole screen (later).
     * If the line is too long, clear it, so ruler and shown command do
     * not get printed in the middle of it.
     */
    msg_check();
    msg_scroll = save_msg_scroll;
    redir_off = FALSE;

    /* When the command line was typed, no need for a wait-return prompt. */
    if (some_key_typed)
	need_wait_return = FALSE;

    State = save_State;
#ifdef USE_IM_CONTROL
    if (b_im_ptr != NULL && *b_im_ptr != B_IMODE_LMAP)
	im_save_status(b_im_ptr);
    im_set_active(FALSE);
#endif
#ifdef FEAT_MOUSE
    setmouse();
#endif
#ifdef CURSOR_SHAPE
    ui_cursor_shape();		/* may show different cursor shape */
#endif

    return ccline.cmdbuff;
}
