/* Client of the server status. Runs on the server machine and connects to the
   burp server to get status information. */

#include "include.h"

#ifdef HAVE_NCURSES_H
#include "ncurses.h"
// So that the sighandler can call endwin():
static enum action actg=ACTION_STATUS;
#endif

#define LEFT_SPACE	3
#define TOP_SPACE	2

#define DBFP	1
#ifdef DBFP
static FILE *dbfp=NULL;
#endif

static void print_line(const char *string, int row, int col)
{
	int k=0;
	const char *cp=NULL;
#ifdef HAVE_NCURSES_H
	if(actg==ACTION_STATUS)
	{
		while(k<LEFT_SPACE) mvprintw(row+TOP_SPACE, k++, " ");
		for(cp=string; (*cp && k<col); cp++)
			mvprintw(row+TOP_SPACE, k++, "%c", *cp);
		while(k<col) mvprintw(row+TOP_SPACE, k++, " ");
		return;
	}
#endif
	while(k<LEFT_SPACE) { printf(" "); k++; }
	for(cp=string; (*cp && k<col); cp++)
		{ printf("%c", *cp); k++; }
	printf("\n");
}

static char *get_bu_str(struct bu *bu)
{
	static char ret[32];
	if(!bu->bno) snprintf(ret, sizeof(ret), "%s", bu->timestamp);
	else snprintf(ret, sizeof(ret), "%07lu %s", bu->bno, bu->timestamp);
	return ret;
}

// Returns 1 if it printed a line, 0 otherwise.
//static int summary(char status, char phase, const char *path,
//	struct cntr *p1cntr, struct cntr *cntr,
//	struct strlist *backups, int count, int row, int col)
static int summary(struct cstat *cstat, int row, int col, struct conf *conf)
{
	char msg[1024]="";
	char fmt[64]="";
	int colwidth=28*((float)col/100);
	snprintf(fmt, sizeof(fmt), "%%-%d.%ds %%-9s %%s%%s",
		colwidth, colwidth);

	switch(cstat->status)
	{
		case STATUS_RUNNING:
/*
		{
			char f[64]="";
			char b[64]="";
			const char *s="";
		  	unsigned long long p=0;
			unsigned long long t=0;

			s=cstat_to_str(phase);
			t=cntr->total+cntr->total_same+cntr->total_changed;
			if(p1cntr->total) p=(t*100)/p1cntr->total;
			snprintf(f, sizeof(f), "%llu/%llu %llu%%",
				t, p1cntr->total, p);
			if(cntr->byte)
				snprintf(b, sizeof(b), "%s",
					bytes_to_human(cntr->byte));
			snprintf(msg, sizeof(msg), fmt,
				c->status, s, f, b);
		}
*/
		case STATUS_IDLE:
		case STATUS_SERVER_CRASHED:
		case STATUS_CLIENT_CRASHED:
		default:
			snprintf(msg, sizeof(msg), fmt,
				cstat->name,
				cstat_status_to_str(cstat),
				" last backup: ",
				get_bu_str(cstat->bu));
				break;
			break;
	}

	if(*msg)
	{
		print_line(msg, row, col);
		return 1;
	}
	return 0;
}
/*
static void show_all_backups(struct strlist *backups, int *x, int col)
{
	char msg[256]="";
	struct strlist *l=NULL;
	struct strlist *last=NULL;
	for(l=backups; l; l=l->next)
	{
		if(!last)
		{
			snprintf(msg, sizeof(msg), "Backup list: %s",
				l->path);
			print_line(msg, (*x)++, col);
		}
		else
		{
			snprintf(msg, sizeof(msg), "             %s",
				l->path);
			print_line(msg, (*x)++, col);
		}
		last=l;
	}
}
*/

/* for the counters */
/*
static void to_msg(char msg[], size_t s, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(msg, s, fmt, ap);
	va_end(ap);
}
*/

/*
static void print_detail(char phase,
	const char *field,
	unsigned long long a,
	unsigned long long b,
	unsigned long long c,
	unsigned long long d,
	unsigned long long e,
	int *x, int col, int percent)
{
	char msg[256]="";
	unsigned long long t=a+b+c;
	if(!field || (!t && !d && !e)) return;

	if(phase==STATUS_RESTORING
	  || phase==STATUS_VERIFYING)
	{
		to_msg(msg, sizeof(msg),
			"% 15s % 9s % 9llu % 9llu",
			field, "", t, e);
	}
	else
	{
		to_msg(msg, sizeof(msg),
			"% 15s % 9llu % 9llu % 9llu % 9llu % 9llu % 9llu",
			field, a, b, c, d, t, e);
	}
	print_line(msg, (*x)++, col);
	if(percent && e)
	{
	  unsigned long long p;
	  p=(t*100)/e;
	  if(phase==STATUS_RESTORING
	    || phase==STATUS_VERIFYING)
	  {
	    to_msg(msg, sizeof(msg), "% 15s % 9s % 9llu%% % 9s",
		"", "", p, "");
	  }
	  else
	  {
	    to_msg(msg, sizeof(msg), "% 15s % 9s % 9s % 9s % 9s % 9llu%% % 9s",
		"", "", "", "", "", p, "");
	  }
	  print_line(msg, (*x)++, col);
	}
}
*/
/*
static void table_header(char phase, int *x, int col)
{
	char msg[256]="";
	if(phase==STATUS_RESTORING
	  || phase==STATUS_VERIFYING)
	{
	  to_msg(msg, sizeof(msg), "% 15s % 9s % 9s % 9s",
	    "", "", "Attempted", "Expected");
	}
	else
	{
	  to_msg(msg, sizeof(msg), "% 15s % 9s % 9s % 9s % 9s % 9s % 9s",
	    "", "New", "Changed", "Unchanged", "Deleted", "Total", "Scanned");
	}
	print_line(msg, (*x)++, col);
}
*/

/*
static void print_detail2(const char *field, unsigned long long value1, const char *value2, int *x, int col)
{
	char msg[256]="";
	if(!field || !value1 || !value2 || !*value2) return;
	snprintf(msg, sizeof(msg), "%s: %llu%s", field, value1, value2);
	print_line(msg, (*x)++, col);
}

static void print_detail3(const char *field, const char *value, int *x, int col)
{
	char msg[256]="";
	if(!field || !value || !*value) return;
	snprintf(msg, sizeof(msg), "%s: %s", field, value);
	print_line(msg, (*x)++, col);
}
*/
/*
static void detail(const char *cntrclient, char status, char phase, const char *path, struct cntr *p1cntr, struct cntr *cntr, struct strlist *backups, int row, int col)
{
	int x=0;
	char msg[1024]="";
	//const char *tmp=NULL;
	if(cntrclient)
	{
		snprintf(msg, sizeof(msg), "Client: %s", cntrclient);
		print_line(msg, x++, col);
	}
	switch(status)
	{
		case STATUS_IDLE:
		{
			print_line("Status: idle", x++, col);
			show_all_backups(backups, &x, col);
			return;
		}
		case STATUS_SERVER_CRASHED:
		{
			print_line("Status: server crashed", x++, col);
			show_all_backups(backups, &x, col);
			return;
		}
		case STATUS_CLIENT_CRASHED:
		{
			print_line("Status: client crashed", x++, col);
			show_all_backups(backups, &x, col);
			return;
		}
		case STATUS_RUNNING:
		{
			if(phase)
			{
				char msg[64]="";
				snprintf(msg, sizeof(msg),
					"Status: running (%s)",
					cstat_to_str(phase));
				print_line(msg, x++, col);
			}
			break;
		}
	}
	print_line("", x++, col);
	table_header(phase, &x, col);

	print_detail(phase, "Files",
				cntr->file,
				cntr->file_changed,
				cntr->file_same,
				cntr->file_deleted,
				p1cntr->file,
				&x, col, 0);
	print_detail(phase, "Encrypted files",
				cntr->enc,
				cntr->enc_changed,
				cntr->enc_same,
				cntr->enc_deleted,
				p1cntr->enc,
				&x, col, 0);
	print_detail(phase, "Meta data",
				cntr->meta,
				cntr->meta_changed,
				cntr->meta_same,
				cntr->meta_deleted,
				p1cntr->meta,
				&x, col, 0);
	print_detail(phase, "Encrypted meta data",
				cntr->encmeta,
				cntr->encmeta_changed,
				cntr->encmeta_same,
				cntr->encmeta_deleted,
				p1cntr->encmeta,
				&x, col, 0);
	print_detail(phase, "Directories",
				cntr->dir,
				cntr->dir_changed,
				cntr->dir_same,
				cntr->dir_deleted,
				p1cntr->dir,
				&x, col, 0);
	print_detail(phase, "Soft links",
				cntr->slink,
				cntr->slink_changed,
				cntr->slink_same,
				cntr->slink_deleted,
				p1cntr->slink,
				&x, col, 0);
	print_detail(phase, "Hard links",
				cntr->hlink,
				cntr->hlink_changed,
				cntr->hlink_same,
				cntr->hlink_deleted,
				p1cntr->hlink,
				&x, col, 0);
	print_detail(phase, "Special files",
				cntr->special,
				cntr->special_changed,
				cntr->special_same,
				cntr->special_deleted,
				p1cntr->special,
				&x, col, 0);
	print_detail(phase, "Total",
				cntr->gtotal,
				cntr->gtotal_changed,
				cntr->gtotal_same,
				cntr->gtotal_deleted,
				p1cntr->gtotal,
				&x, col, 1);
	print_line("", x++, col);
	print_detail(phase, "Warnings",
				cntr->warning, 0, 0, 0, 0,
				&x, col, 1);

	if(p1cntr->byte)
	{
		tmp=bytes_to_human(p1cntr->byte);
		print_detail2("Bytes estimated", p1cntr->byte, tmp, &x, col);
	}
	if(cntr->byte)
	{
		const char *text=NULL;
		if(phase==STATUS_BACKUP) text="Bytes in backup";
		else if(phase==STATUS_RESTORING) text="Bytes attempted";
		else if(phase==STATUS_VERIFYING) text="Bytes checked";
		tmp=bytes_to_human(cntr->byte);
		if(text) print_detail2(text, cntr->byte, tmp, &x, col);
	}
	if(cntr->recvbyte)
	{
		const char *text=NULL;
		tmp=bytes_to_human(cntr->recvbyte);
		if(phase==STATUS_BACKUP) text="Bytes received";
		if(text) print_detail2(text, cntr->recvbyte, tmp, &x, col);
	}
	if(cntr->sentbyte)
	{
		const char *text=NULL;
		if(phase==STATUS_BACKUP) text="Bytes sent";
		else if(phase==STATUS_RESTORING) text="Bytes sent";
		tmp=bytes_to_human(cntr->sentbyte);
		print_detail2(text, cntr->sentbyte, tmp, &x, col);
	}
	if(p1cntr->start)
	{
		time_t now=0;
		time_t diff=0;
		now=time(NULL);
		diff=now-p1cntr->start;

		print_detail3("Start time", getdatestr(p1cntr->start), &x,col);
		print_detail3("Time taken", time_taken(diff), &x, col);

		if(diff>0)
		{
			unsigned long long bytesleft=0;
			unsigned long long byteswant=0;
			unsigned long long bytesgot=0;
			float bytespersec=0;
			byteswant=p1cntr->byte;
			bytesgot=cntr->byte;
			bytespersec=(float)(bytesgot/diff);
			bytesleft=byteswant-bytesgot;
			if(bytespersec>0)
			{
				time_t timeleft=0;
				timeleft=(time_t)(bytesleft/bytespersec);
				print_detail3("Time left",
					time_taken(timeleft), &x, col);
			}
		}
	}
	if(path && *path)
	{
#ifdef HAVE_NCURSES_H
		if(actg==ACTION_STATUS)
		{
			printw("\n%s\n", path);
			return;
		}
#else
		printf("\n%s\n", path);
#endif
	}
}
*/

static void blank_screen(int row, int col)
{
	int c=0;
	int l=0;
	const char *date=NULL;
	time_t t=time(NULL);
	date=getdatestr(t);
	l=strlen(date);
#ifdef HAVE_NCURSES_H
	if(actg==ACTION_STATUS)
	{
		char v[32]="";
	// Try not actually blanking it - should stop screen flicker.
	//	clear();
		snprintf(v, sizeof(v), " burp monitor %s", VERSION);
		print_line(v, 0-TOP_SPACE, col);
	//	mvprintw(0, 0, v);
		mvprintw(0, col-l-1, date);
		return;
	}
#endif

	printf("\n burp status");

	for(c=0; c<(int)(col-strlen(" burp status")-l-1); c++) printf(" ");
	printf("%s\n\n", date);
}

/*
static int parse_rbuf(const char *rbuf, struct conf *conf, int row, int col, int sel, char **client, int *count, int details, const char *sclient, struct cntr *p1cntr, struct cntr *cntr)
{
	char *cp=NULL;
	char *dp=NULL;
	char *copy=NULL;

	if(!(copy=strdup_w(rbuf, __func__)))
		return -1;

	dp=copy;
	*count=0;

	// First, blank the whole screen.
	blank_screen(row, col);
	while((cp=strchr(dp, '\n')))
	{
		char status='\0';
		char phase='\0';
		char *path=NULL;
		struct strlist *backups=NULL;
		char *cntrclient=NULL;
		*cp='\0';

		if(str_to_cntr(dp, &cntrclient, &status, &phase, &path,
			p1cntr, cntr, &backups))
		{
			free(copy);
			if(path) free(path);
			if(cntrclient) free(cntrclient);
			return -1;
		}

		if(!cntrclient) continue;

		if(details)
		{
			if(*count==sel || sclient)
			{
				if(cntrclient
				  && (!*client
				    || strcmp(cntrclient, *client)))
				{
					if(*client) free(*client);
					*client=strdup_w(cntrclient, __func__);
				}
				if(!sclient
				  || (cntrclient
				    && !strcmp(cntrclient, sclient)))
					detail(cntrclient, status, phase,
						path, p1cntr, cntr,
						backups, 0, col);
			}
		}
		else
		{
			summary(cntrclient, status, phase,
				path, p1cntr, cntr, backups,
				*count, row, col);
		}
		(*count)++;

		dp=cp+1;
		if(path) free(path);
		if(cntrclient) free(cntrclient);
		strlists_free(&backups);
	}
	if(copy) free(copy);
	return 0;
}
*/

static int need_status(void)
{
	static time_t lasttime=0;
	time_t now=0;
	time_t diff=0;

	// Only ask for an update every second.
	now=time(NULL);
	diff=now-lasttime;
	if(diff<1)
	{
		// In case they fiddled their clock back in time.
		if(diff<0) lasttime=now;
		return 0;
	}
	lasttime=now;
	return 1;
}

// Return 1 if it was shown, -1 on error, 0 otherwise.
/*
static int show_rbuf(const char *rbuf, struct conf *conf, int sel, char **client, int *count, int details, const char *sclient, struct cntr *p1cntr, struct cntr *cntr)
{
	int rbuflen=0;
	if(!rbuf) return 0;
	rbuflen=strlen(rbuf);
#ifdef DBFP
	if(dbfp) { fprintf(dbfp, "%s\n", rbuf);  fflush(dbfp); }
#endif

	if(rbuflen>2
		&& rbuf[rbuflen-1]=='\n'
		&& rbuf[rbuflen-2]=='\n')
	{
		int row=24;
		int col=80;
#ifdef HAVE_NCURSES_H
		if(actg==ACTION_STATUS) getmaxyx(stdscr, row, col);
#endif
		if(parse_rbuf(rbuf, conf, row, col,
			sel, client, count, details, sclient, p1cntr, cntr))
				return -1;
		if(sel>=*count) sel=(*count)-1;
		if(!details) print_star(sel);
#ifdef HAVE_NCURSES_H
		if(actg==ACTION_STATUS) refresh();
#endif
		return 1;
	}
	return 0;
}
*/

static int update_screen_details(struct cstat *clist,
	struct cstat *sel, struct conf *conf)
{
	int x=0;
	int row=24;
	int col=80;
	char msg[1024]="";
	struct bu *bu;

	getmaxyx(stdscr, row, col);
	blank_screen(row, col);
	snprintf(msg, sizeof(msg), "Client: %s", sel->name);
	print_line(msg, x++, col);
	snprintf(msg, sizeof(msg), "Status: %s", cstat_status_to_str(sel));
	print_line(msg, x++, col);

	if(sel->bu)
	{
		snprintf(msg, sizeof(msg), "Backup list: %s",
			get_bu_str(sel->bu));
		print_line(msg, x++, col);
		for(bu=sel->bu->next; bu; bu=bu->next)
		{
			snprintf(msg, sizeof(msg), "             %s",
				get_bu_str(bu));
			print_line(msg, x++, col);
		}
	}

	//show_all_backups(backups, &x, col);
	// Blank any remainder of the screen.
	for(; x<row; x++)
		print_line("", x, col);
	return 0;
}

static int update_screen_summary(struct cstat *clist,
	struct cstat *sel, struct conf *conf)
{
	int x=0;
	struct cstat *c;
#ifdef HAVE_NCURSES_H
	int s=0;
	int row=24;
	int col=80;
	int selindex=0;
	static int winmin=0;
	static int winmax=0;
	static int selindex_last=0;
	if(actg==ACTION_STATUS)
	{
		getmaxyx(stdscr, row, col);
		//if(!winmax) winmax=row;
		for(c=clist; c; c=c->next)
		{
			selindex++;
			if(sel==c) break;
		}
	}

	// Adjust sliding window appropriately.
	if(selindex>selindex_last)
	{
		if(selindex>winmax-TOP_SPACE-1)
		{
			winmin+=selindex-selindex_last;
			winmax+=selindex-selindex_last;
		}
	}
	else if(selindex<selindex_last)
	{
		if(selindex<winmin)
		{
			winmin+=selindex-selindex_last;
			winmax+=selindex-selindex_last;
		}
	}
	if(winmin==winmax)
	{
		winmin=0;
		winmax=row;
	}

	// First, blank the whole screen.
	blank_screen(row, col);
/*
	{
		char msg[64];
		snprintf(msg, sizeof(msg), "sel:%d si:%d min:%d max:%d\n",
			selindex, selindex_last, winmin, winmax);
		print_line(msg, -1, col);
	}
*/
#endif
	for(c=clist; c; c=c->next)
	{
#ifdef HAVE_NCURSES_H
		s++;
		if(s<winmin) continue;
		if(s>winmax) break;

		summary(c, x++, col, conf);

		if(actg==ACTION_STATUS && sel==c)
			mvprintw(x+TOP_SPACE-1, 1, "*");
#endif
	}
	// Blank any remainder of the screen.
	for(; x<row; x++)
		print_line("", x, col);

	selindex_last=selindex;
	
	return 0;
}

static int request_status(struct asfd *asfd,
	const char *client, struct conf *conf)
{
	char buf[256]="";
	if(client)
	{
		if(conf->backup)
		{
			if(conf->browsedir)
				snprintf(buf, sizeof(buf), "c:%s:b:%s:p:%s\n",
					client, conf->backup, conf->browsedir);
			else if(conf->browsefile)
				snprintf(buf, sizeof(buf), "c:%s:b:%s:f:%s\n",
					client, conf->backup, conf->browsefile);
			else
				snprintf(buf, sizeof(buf), "c:%s:b:%s\n",
					client, conf->backup);
		}
		else
			snprintf(buf, sizeof(buf), "c:%s\n", client);
	}
	else
		snprintf(buf, sizeof(buf), "\n");
#ifdef DBFP
fprintf(dbfp, "request: %s\n", buf);
#endif
	if(asfd->write_str(asfd, CMD_GEN /* ignored */, buf)) return -1;
	return 0;
}

static void sighandler(int sig)
{
#ifdef HAVE_NCURSES_H
	if(actg==ACTION_STATUS) endwin();
#endif
        logp("got signal: %d\n", sig);
	if(sig==SIGPIPE) logp("Server may have too many active status clients.\n");
        logp("exiting\n");
        exit(1);
}

static void setup_signals(void)
{
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
	signal(SIGINT, &sighandler);
	signal(SIGPIPE, &sighandler);
}

// FIX THIS: Identical function in status_server.c and probably elsewhere.
static int setup_asfd(struct async *as, const char *desc, int *fd,
	enum asfd_streamtype asfd_streamtype, struct conf *conf)
{
	struct asfd *asfd=NULL;
	if(!fd || *fd<0) return 0;
	set_non_blocking(*fd);
	if(!(asfd=asfd_alloc())
	  || asfd->init(asfd, desc, as, *fd, NULL, asfd_streamtype, conf))
		goto error;
	*fd=-1;
	as->asfd_add(as, asfd);
	return 0;
error:
	asfd_free(&asfd);
	return -1;
}

#ifdef HAVE_NCURSES_H
static int parse_stdin_data(struct asfd *asfd,
	struct cstat **sel, int *details, int count, int *enterpressed)
{
	static int ch;
	if(asfd->rbuf->len!=sizeof(ch))
	{
		logp("Unexpected input length in %s: %d\n",
			__func__, asfd->rbuf->len);
		return -1;
	}
	memcpy(&ch, asfd->rbuf->buf, sizeof(ch));
	switch(ch)
	{
		case 'q':
		case 'Q':
			return 1;
		case KEY_UP:
		case 'k':
		case 'K':
			if(*details) break;
			if(*sel && (*sel)->prev) *sel=(*sel)->prev;
			break;
		case KEY_DOWN:
		case 'j':
		case 'J':
			if(*details) break;
			if(*sel && (*sel)->next) *sel=(*sel)->next;
			break;
		case KEY_ENTER:
		case '\n':
		case ' ':
			if(*details) *details=0;
			else (*details)++;
			enterpressed++;
			break;
		case KEY_LEFT:
		case 'h':
		case 'H':
			*details=0;
			break;
		case KEY_RIGHT:
		case 'l':
		case 'L':
			(*details)++;
			break;
		case KEY_PPAGE:
		{
			int row=0;
			int col=0;
			struct cstat *c;
			if(!*sel) break;
			getmaxyx(stdscr, row, col);
			for(c=*sel; c; c=c->prev)
			{
				row--;
				if(!row) break;
			}
			*sel=c;
			break;
		}
		case KEY_NPAGE:
		{
			int row=0;
			int col=0;
			struct cstat *c;
			if(!*sel) break;
			getmaxyx(stdscr, row, col);
			for(c=*sel; c; c=c->next)
			{
				row--;
				if(!row) break;
				if(!c->next) break;
			}
			*sel=c;
			break;
		}
		case -1:
			logp("Error on stdin\n");
			return -1;
	}

	return 0;
}
#endif

static int parse_data(struct asfd *asfd, struct cstat **clist,
	struct cstat **sel, int *details, int count, int *enterpressed)
{
	// Hacky to switch on whether it is using char buffering or not.
#ifdef HAVE_NCURSES_H
	if(actg==ACTION_STATUS && asfd->streamtype==ASFD_STREAM_NCURSES_STDIN)
		return parse_stdin_data(asfd,
			sel, details, count, enterpressed);
#endif
	return json_input(asfd, clist);
}

static int main_loop(struct async *as, const char *sclient, struct conf *conf)
{
	int ret=-1;
	char *client=NULL;
	int details=0;
	int count=0;
	struct asfd *asfd=NULL;
	struct cstat *clist=NULL;
	struct cstat *sel=NULL;
	int enterpressed=0;
	struct asfd *sfd=as->asfd; // Server asfd.
	int reqdone=0;

	if(sclient && !client)
	{
		client=strdup_w(sclient, __func__);
		details=1;
	}

	while(1)
	{
		if((enterpressed || need_status()) && !reqdone)
		{
			char *req=NULL;
			if(details)
			{
				if(client) req=client;
				else if(sel) req=sel->name;
			}
			if(request_status(sfd, req, conf)) goto error;
			enterpressed=0;
			if(actg==ACTION_STATUS_SNAPSHOT)
				reqdone=1;
		}

		if(as->read_write(as))
		{
			// FIX THIS - an exception is thrown when the console
			// is resized.
			if(sfd->want_to_remove)
			{
				sfd->want_to_remove=0;
				continue;
			}
			logp("Exiting main loop\n");
			break;
		}

		for(asfd=as->asfd; asfd; asfd=asfd->next)
			while(asfd->rbuf->buf)
		{
			if(parse_data(asfd, &clist,
				&sel, &details, count, &enterpressed))
					goto error;
			iobuf_free_content(asfd->rbuf);
			if(asfd->parse_readbuf(asfd))
				goto error;
		}

		if(!sel) sel=clist;
//if(sel) fprintf(dbfp, "sel: %s\n", sel->name);
		if(details)
		{
			if(update_screen_details(clist, sel, conf)) return -1;
		}
		else
		{
			if(update_screen_summary(clist, sel, conf)) return -1;
		}
		refresh();
	}

	ret=0;
error:
	return ret;
}

#ifdef HAVE_NCURSES_H
static int init_pair_w(int x)
{
	return init_pair(x, COLOR_WHITE, COLOR_BLACK);
}

static void ncurses_init(void)
{
	initscr();
	start_color();
	init_pair_w(1);
	init_pair_w(2);
	init_pair_w(3);
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(0);
	halfdelay(3);
	//nodelay(stdscr, TRUE);
}
#endif

int status_client_ncurses(enum action act, const char *sclient,
	struct conf *conf)
{
	int fd=0;
        int ret=-1;
	struct async *as=NULL;

#ifdef HAVE_NCURSES_H
	actg=act; // So that the sighandler can call endwin().
#else
	if(act==ACTION_STATUS)
	{
		printf("To use the live status monitor, you need to recompile with ncurses support.\n");
		return -1;
	}
#endif

	setup_signals();

	// NULL == ::1 or 127.0.0.1.
	if((fd=init_client_socket(NULL, conf->status_port))<0)
		return -1;
	set_non_blocking(fd);

	if(!(as=async_alloc())
	  || as->init(as, 0)
	  || setup_asfd(as, "status socket",
		&fd, ASFD_STREAM_LINEBUF, conf))
			goto end;
#ifdef HAVE_NCURSES_H
	if(actg==ACTION_STATUS)
	{
		int stdinfd=fileno(stdin);
		if(setup_asfd(as, "stdin",
			&stdinfd, ASFD_STREAM_NCURSES_STDIN, conf))
				goto end;
		ncurses_init();
	}
#endif
#ifdef DBFP
	dbfp=fopen("/tmp/dbfp", "w");
#endif
	// FIX THIS: Maybe think of a way to buffer error messages and print
	// them all after the monitor has exited.
	//set_logfp_direct(stderr);
	set_logfp_direct(dbfp);

	ret=main_loop(as, sclient, conf);
end:
#ifdef HAVE_NCURSES_H
	if(actg==ACTION_STATUS) endwin();
#endif
#ifdef DBFP
	if(dbfp) fclose(dbfp);
#endif
	async_asfd_free_all(&as);
	close_fd(&fd);
	return ret;
}
