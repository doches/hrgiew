#include "progressbar.h"

progressbar *progressbar_new(const char *label,unsigned int max)
{
	progressbar *bar = (progressbar *)malloc(sizeof(progressbar));
	bar->label = label;
	bar->max = max;
	bar->value = 0;
	bar->start = time(0);
	bar->steps = 80 - (strlen(label) + 14);
	bar->step = 0;
	bar->progress_str = (char *)malloc(sizeof(char)*(bar->steps+1));
	bar->format = (char *)malloc(sizeof(char)*3);
	bar->format[0] = '|';
	bar->format[1] = 'o';
	bar->format[2] = '|';
	memset(bar->progress_str,' ',bar->steps);
	bar->progress_str[bar->steps] = '\0';
	bar->last_printed = 0;
    
  progressbar_draw(bar, 0);
	
	return bar;
}

void progressbar_free(progressbar *bar)
{
	// We malloc'd a couple of strings, so let's be sure to free those...
	free(bar->progress_str);
	free(bar->format);
	// ...before we free the struct itself.
	free(bar);
	
	return;
}

void progressbar_inc(progressbar *bar)
{
	bar->value++;
	unsigned int current_step = (unsigned int)((bar->value/(float)bar->max) * bar->steps);
	
	// Only redraw the progressbar if the visual progress display (the current 'step')
	// has changed. 
	if(current_step != bar->step) {
		// Fill the bar to the current step...
		for(unsigned int i=bar->step;i<current_step;i++) {
			bar->progress_str[i] = bar->format[1];
		}
		bar->step = current_step;
		
		// Draw using a rough estimated time remaining.
		// Time remaining is estimated quite roughly, as the number of increments 
		// remaining * the average time per increment thus far.
		unsigned int offset = time(0) - (bar->start);
		unsigned int estimate = (unsigned int)((offset/(float)bar->value) * (bar->max - bar->value));
		progressbar_draw(bar,estimate);
	}
	
	return;
}

void progressbar_draw(progressbar *bar,unsigned int time)
{
	// Erase the last draw. If anything else has been printed to stderr,
	// things are going to look mighty interesting...
	for(int i=0;i<bar->last_printed;i++) {
		fprintf(stderr,"\b");
	}
	
	// Convert the time to display into HHH:MM:SS
	unsigned int h = time/3600;
	time -= h*3600;
	unsigned int m = time/60;
	time -= m*60;
	unsigned int s = time;
	// ...and display!
	fprintf(stderr,"%s: %c%s%c %3d:%02d:%02d%n",bar->label,bar->format[0],bar->progress_str,bar->format[2],h,m,s,&(bar->last_printed));
	
	return;
}

void progressbar_finish(progressbar *bar)
{
	// Draw one more time, with the actual time to completion instead of a useless
	// 00:00:00 remaining estimate.
	unsigned int offset = time(0) - (bar->start);
	// Make sure we fill the progressbar too, so things look complete.
	for(unsigned int i=bar->step;i<bar->steps;i++) {
		bar->progress_str[i] = bar->format[1];
	}
	progressbar_draw(bar,offset);
	
	// Print a newline, so that future outputs to stderr look prettier
	fprintf(stderr,"\n");
	
	// We've finished with this progressbar, so go ahead and free it.
//	progressbar_free(bar);
	
	return;
}
