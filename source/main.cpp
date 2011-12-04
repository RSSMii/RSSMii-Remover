#include <wiiuse/wpad.h>
#include <wc24/wc24.h>
#include <stdio.h>

static void *xfb = NULL;
static GXRModeObj *rmode = NULL;

nwc24dl_record myrec;
nwc24dl_entry myent;

void remove()
{
	u64 homebrewtitleid = 0x0001000846454544LL; //TitleID for RSS-Feed mail: 00010008-FEED.
	printf("Initializing WC24...\n");

	s32 retval = WC24_Init();
	if(retval<0)
	{
		printf("WC24_Init returned %d\n", retval);
		return;
	}

	printf("Deleting...\n");

	int i = 0;
	while((retval=WC24_FindRecord((u32)homebrewtitleid, &myrec))!=LIBWC24_ENOENT)
	{
		WC24_DeleteRecord((u32)retval);
		i++;
	}

	printf("Deleted %i entries.\n", i);

	printf("Shutting down WC24...\n");
	retval = WC24_Shutdown();
	if(retval<0)
	{
		printf("WC24_Shutdown returned %d\n", retval);
		return;
	}
}

int main(int argc, char **argv) {
	// Initialise the video system
	VIDEO_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	printf("\x1b[2;0H");
	printf("Welcome to the rssmii-remover!\n\n");

	remove();
	printf("Done.");
	return 0;
}