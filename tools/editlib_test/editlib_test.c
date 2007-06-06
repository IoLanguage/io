/*
  Copyright Jonathan Wright 2007
  License: BSD revised
*/

#include <stdio.h>
#include <histedit.h>

char prompt[] = "test> ";

char *promptCallback(EditLine *e)
{
	return prompt;
}

int main()
{
	History *h = history_init();
	EditLine *e = el_init("edittest", stdin, stdout, stderr);
	el_set(e, EL_PROMPT, promptCallback);
	el_set(e, EL_HIST, history, h);
	el_set(e, EL_SIGNAL, 1);
	el_set(e, EL_EDITOR, "emacs");

	{
		HistEvent ev;
		history(h, &ev, H_SETSIZE, 1024);
	}

	for (;;)
	{
		int count = 0;
		const char *str = el_gets(e, &count);

		if (!str || count <= 0)
			break;

		puts(str);

		{
			HistEvent ev;
			history(h, &ev, H_ENTER, str);
		}
	}

	el_end(e);
	history_end(h);
	return 0;
}
